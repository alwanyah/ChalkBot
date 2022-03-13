#include "WebApi.h"

#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include "../BB.h"
#include "../util/Logger.h"

static Logger logger("WebApi");

static void sendBadRequest(AsyncWebServerRequest *request, const char *extra);
static void sendNotFound(AsyncWebServerRequest *request);
static void sendMethodNotAllowed(AsyncWebServerRequest *request);
static void sendPayloadTooLarge(AsyncWebServerRequest *request);
static void sendUnsupportedMediaType(AsyncWebServerRequest *request);

class GetHandler : public AsyncWebHandler {
public:
    using GetFunction = void (*)(AsyncWebServerRequest *request);

    GetHandler(const String &url, GetFunction getFunction)
        : url(url)
        , getFunction(getFunction)
    {}

    virtual bool canHandle(AsyncWebServerRequest *request) override {
        return request->url() == url;
    }

    virtual void handleRequest(AsyncWebServerRequest *request) override {
        if (request->method() == HTTP_GET) {
            getFunction(request);
        } else {
            sendMethodNotAllowed(request);
        }
    }

    virtual bool isRequestHandlerTrivial() override {
        return false;
    }

private:
    String url;
    GetFunction getFunction;
};

class GetPatchHandler : public GetHandler {
    static constexpr size_t MAX_CONTENT_LENGTH = 2048;
    static constexpr size_t JSON_SIZE = 2048;

public:
    using PatchFunction = void (*)(AsyncWebServerRequest *request, const JsonDocument &doc);

    GetPatchHandler(const String &url, GetFunction getFunction, PatchFunction patchFunction)
        : GetHandler(url, getFunction)
        , patchFunction(patchFunction)
    {}

    virtual ~GetPatchHandler() override {
        if (content != nullptr) {
            delete[] content;
            content = nullptr;
        }
    }

    virtual void handleRequest(AsyncWebServerRequest *request) override {
        if (request->contentLength() > MAX_CONTENT_LENGTH) {
            sendPayloadTooLarge(request);
        } else if (request->method() == HTTP_OPTIONS) {
            AsyncWebServerResponse *response = request->beginResponse(204);
            response->addHeader("Access-Control-Allow-Origin", "*");
            response->addHeader("Access-Control-Allow-Methods", "GET, PATCH, OPTIONS");
            response->addHeader("Access-Control-Allow-Headers", "Content-Type");
            response->addHeader("Access-Control-Max-Age", "3600");
            request->send(response);
        } else if (request->method() == HTTP_PATCH) {
            if (request->contentType() == "application/json") {
                DynamicJsonDocument doc(JSON_SIZE);
                DeserializationError error = deserializeJson(doc, content, request->contentLength());
                if (error) {
                    sendBadRequest(request, error.c_str());
                } else {
                    patchFunction(request, doc);
                }
            } else {
                sendUnsupportedMediaType(request);
            }
        } else {
            GetHandler::handleRequest(request);
        }

        if (content != nullptr) {
            delete[] content;
            content = nullptr;
        }
    }

    virtual void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) override {
        if (total > MAX_CONTENT_LENGTH) {
            // noop
        } else if (index == 0) {
            content = new uint8_t[total];
            memcpy(content, data, len);
        } else {
            memcpy(content + index, data, len);
        }
    }

    virtual bool isRequestHandlerTrivial() override {
        return false;
    }

private:
    PatchFunction patchFunction;
    uint8_t *content = nullptr;
};

static void sendJson(AsyncWebServerRequest *request, int code, const JsonDocument &doc) {
    if (doc.overflowed()) {
        logger.log_error("JSON overflowed");
        request->send(500);
        return;
    }

    // TODO: serialize as msgpack if requested (by Accept header or ?format=msgpack), see serializeMsgPack
    String body;
    body.reserve(measureJson(doc));
    serializeJson(doc, body);

    AsyncWebServerResponse *response = request->beginResponse(code, "application/json", body);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

static void sendGenericResponse(AsyncWebServerRequest *request, int code, const char *message, const char *extra = nullptr) {
    static constexpr size_t JSON_SIZE = JSON_OBJECT_SIZE(3);

    StaticJsonDocument<JSON_SIZE> doc;
    JsonObject root = doc.to<JsonObject>();

    root["code"] = code;
    root["message"] = message;
    if (extra != nullptr) {
        root["extra"] = extra;
    }

    sendJson(request, code, doc);
}

static void sendBadRequest(AsyncWebServerRequest *request, const char *extra) {
    sendGenericResponse(request, 400, "Bad Request", extra);
}

static void sendNotFound(AsyncWebServerRequest *request) {
    sendGenericResponse(request, 404, "Not Found");
}

static void sendMethodNotAllowed(AsyncWebServerRequest *request) {
    sendGenericResponse(request, 405, "Method Not Allowed");
}

static void sendPayloadTooLarge(AsyncWebServerRequest *request) {
    sendGenericResponse(request, 413, "Payload Too Large");
}

static void sendUnsupportedMediaType(AsyncWebServerRequest *request) {
    sendGenericResponse(request, 415, "Unsupported Media Type");
}

static void sensorsHandler(AsyncWebServerRequest *request) {
    static constexpr size_t JSON_SIZE =
        JSON_OBJECT_SIZE(2) // root
            + JSON_OBJECT_SIZE(4) // imu
                + JSON_OBJECT_SIZE(4) // calibration
            + JSON_OBJECT_SIZE(5) // gnss
                + JSON_OBJECT_SIZE(4) // global
                + JSON_OBJECT_SIZE(5) // relative
                + JSON_OBJECT_SIZE(7) // motion
    ;

    StaticJsonDocument<JSON_SIZE> doc; // should maybe be a DynamicJsonDocument, if this grows too large
    JsonObject root = doc.to<JsonObject>();

    if (!bb::imu.isInitialized()) {
        root["imu"] = nullptr;
    } else {
        JsonObject imu = root.createNestedObject("imu");
        imu["heading"] = bb::imu.getHeading();
        imu["roll"] = bb::imu.getRoll();
        imu["pitch"] = bb::imu.getPitch();

        {
            JsonObject calibration = imu.createNestedObject("calibration");
            calibration["system"] = bb::imu.getSystemCalibration();
            calibration["gyroscope"] = bb::imu.getGyroscopeCalibration();
            calibration["accelerometer"] = bb::imu.getAccelerometerCalibration();
            calibration["magnetometer"] = bb::imu.getMagnetometerCalibration();
        }
    }

    if (!bb::gnss.isInitialized()) {
        root["gnss"] = nullptr;
    } else {
        JsonObject gnss = root.createNestedObject("gnss");
        gnss["ntripConnected"] = bb::gnss.isNtripConnected();
        gnss["satellites"] = bb::gnss.getSatellites();

        {
            JsonObject global = gnss.createNestedObject("global");
            global["timestamp"] = bb::gnss.getGlobalTimestamp();
            global["latitude"] = bb::gnss.getLatitude();
            global["longitude"] = bb::gnss.getLongitude();
            global["horizontalAccuracy"] = bb::gnss.getHorizontalAccuracy();
        }

        {
            JsonObject relative = gnss.createNestedObject("relative");
            relative["timestamp"] = bb::gnss.getRelativeTimestamp();
            relative["north"] = bb::gnss.getNorth();
            relative["east"] = bb::gnss.getEast();
            relative["northAccuracy"] = bb::gnss.getNorthAccuracy();
            relative["eastAccuracy"] = bb::gnss.getEastAccuracy();
        }

        {
            JsonObject motion = gnss.createNestedObject("motion");
            motion["timestamp"] = bb::gnss.getMotionTimestamp();
            motion["north"] = bb::gnss.getNorthVelocity();
            motion["east"] = bb::gnss.getEastVelocity();
            motion["speed"] = bb::gnss.getSpeed();
            motion["heading"] = bb::gnss.getHeading();
            motion["speedAccuracy"] = bb::gnss.getSpeedAccuracy();
            motion["headingAccuracy"] = bb::gnss.getHeadingAccuracy();
        }
    }

    sendJson(request, 200, doc);
}

static void poseGetHandler(AsyncWebServerRequest *request) {
    static constexpr size_t JSON_SIZE = JSON_OBJECT_SIZE(4);

    StaticJsonDocument<JSON_SIZE> doc;
    JsonObject root = doc.to<JsonObject>();

    root["north"] = bb::poseFusion.getNorth();
    root["east"] = bb::poseFusion.getEast();
    root["heading"] = bb::poseFusion.getHeading();
    root["useGnss"] = bb::poseFusion.isUsingGnss();

    sendJson(request, 200, doc);
}

static void posePatchHandler(AsyncWebServerRequest *request, const JsonDocument &requestDoc) {
    JsonObjectConst requestRoot = requestDoc.as<JsonObjectConst>();

    JsonVariantConst useGnss = requestRoot["useGnss"];
    if (!useGnss.isUnbound()) {
        if (!useGnss.is<bool>()) {
            sendBadRequest(request, "useGnss has wrong type (should be bool)");
            return;
        }
        bb::poseFusion.setUseGnss(useGnss);
    }

    StaticJsonDocument<0> doc;
    doc.to<JsonObject>();
    sendJson(request, 200, doc);
}

static void actionQueueGetHandler(AsyncWebServerRequest *request) {
    static constexpr size_t JSON_SIZE = JSON_OBJECT_SIZE(2)
        + JSON_ARRAY_SIZE(config::ACTION_QUEUE_CAPACITY)
        + config::ACTION_QUEUE_CAPACITY * Action::JSON_SIZE;

    StaticJsonDocument<JSON_SIZE> doc;
    JsonObject root = doc.to<JsonObject>();

    root["capacity"] = config::ACTION_QUEUE_CAPACITY;
    JsonArray queue = root.createNestedArray("queue");
    for (const Action &action : bb::behavior.getActions()) {
        JsonObject object = queue.createNestedObject();
        action.serialize(object);
    }

    sendJson(request, 200, doc);
}

static void actionQueuePatchHandler(AsyncWebServerRequest *request, const JsonDocument &requestDoc) {
    JsonObjectConst requestRoot = requestDoc.as<JsonObjectConst>();

    String method = requestRoot["method"];
    JsonArrayConst queue = requestRoot["queue"];
    if (queue.isNull()) {
        sendBadRequest(request, "bad action queue");
        return;
    }

    std::vector<Action> actions;
    actions.reserve(config::ACTION_QUEUE_CAPACITY);
    for (const JsonObjectConst &object : queue) {
        if (actions.size() == config::ACTION_QUEUE_CAPACITY) {
            sendBadRequest(request, "action queue too long");
            return;
        }
        Action action = Action::deserialize(object);
        if (action.getType() == Action::ERROR) {
            sendBadRequest(request, "bad action in queue");
            return;
        }
        actions.push_back(action);
    }

    static constexpr size_t JSON_SIZE = JSON_OBJECT_SIZE(1);

    StaticJsonDocument<JSON_SIZE> responseDoc;
    JsonObject responseRoot = responseDoc.to<JsonObject>();

    if (method == "replace") {
        bb::behavior.setActions(std::move(actions));
    } else if (method == "append") {
        size_t appendCount = config::ACTION_QUEUE_CAPACITY - bb::behavior.getActions().size();
        if (actions.size() > appendCount) {
            actions.resize(appendCount);
        } else {
            appendCount = actions.size();
        }
        bb::behavior.pushActions(std::move(actions));
        responseRoot["appended"] = appendCount;

    } else {
        sendBadRequest(request, "bad method");
        return;
    }

    sendJson(request, 200, responseDoc);
}

void WebApi::begin() {
    server.onNotFound(sendNotFound);
    server.addHandler(new GetHandler("/sensors", sensorsHandler));
    server.addHandler(new GetPatchHandler("/pose", poseGetHandler, posePatchHandler));
    server.addHandler(new GetPatchHandler("/action_queue", actionQueueGetHandler, actionQueuePatchHandler));
    server.begin();
}
