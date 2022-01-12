
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

//https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncWebServer.h>

#include "../BB.h"
#include "../util/math/Vector2.h"

class WebServer
{
  AsyncWebServer server;

  // log messages (experimental)
  // FIXME: integrate this with logger? Memory might get full if we store infinite logs.
  String message = "";

  // for parsing commands
  char receive_buffer[128];

  // FIXME: replace with logger
  int16_t debug = 0;

public:

  WebServer() : server(80) {}

  void begin() {
    registerRequests();
    server.begin();
  }

  void update();

private:
  char* make_string(uint8_t* inputData, size_t len) {
    assert(len < sizeof(receive_buffer));
    memcpy(receive_buffer, inputData, len);
    receive_buffer[len] = '\0';
    return receive_buffer;
  }

  void registerRequests()
  {
    // register get responses
    server.on("/log", HTTP_GET, [&](AsyncWebServerRequest *request){
      request->send(200, "text/plain", message);
    });

    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
      String str;
      str += "<p>ChalkBot Control. Use apropriate get requests.</p>";
      str += "<p>IMU: " + getImuStatusString() + "</p>";
      request->send(200, "text/plain", str);
    });

    server.on("/status_motion", HTTP_POST, [&](AsyncWebServerRequest *request) {
      String status_motion = getStatusMotionString();
      Serial.println("/status_motion");
      Serial.println(status_motion);
      request->send(200, "text/plain", status_motion);
    });

    server.on("/status_imu", HTTP_POST, [&](AsyncWebServerRequest *request) {
      String status_imu = getImuStatusString();
      Serial.println("/status_imu");
      Serial.println(status_imu);
      request->send(200, "text/plain", status_imu);
    });

    server.on("/orientation", HTTP_POST, [&](AsyncWebServerRequest *request) {
      Serial.println("/orientation");
      Serial.println(bb::imu.getOrientation());
      request->send(200, "text/plain", String(bb::imu.getOrientation()));
    });

    server.on("/pose", HTTP_POST, [&](AsyncWebServerRequest *request) {
      Pose2D robotPose = bb::odometry.getRobotPose();
      Serial.println("/pose");
      String msg = String("")
        + robotPose.translation.x + ";"
        + robotPose.translation.y + ";"
        + robotPose.rotation;

      request->send(200, "text/plain", msg);
    });

    server.on("/debug", HTTP_POST, [&](AsyncWebServerRequest *request) {}, NULL,
      [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
        sscanf(make_string(data_website, len), "%" SCNd16, &debug);

        Serial.println("/debug");
        Serial.println(debug);
        request->send(200);
      }
    );

    server.on("/motor", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
      [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
        Serial.println("/motor");

        sscanf(make_string(data_website, len), "%" SCNd16 ";%" SCNd16 ";%" SCNd16 ";%" SCNd16 ";%" SCNd16,
          &bb::webServer.motorTestRequest.motorFrontLeftSpeed,
          &bb::webServer.motorTestRequest.motorFrontRightSpeed,
          &bb::webServer.motorTestRequest.motorRearLeftSpeed,
          &bb::webServer.motorTestRequest.motorRearRightSpeed,
          &bb::webServer.motorTestRequest.printerSpeed
        );

        bb::webServer.timeOfLastCommand = millis();
        bb::webServer.lastCommand = Command::MOTOR_TEST;

        request->send(200);
      }
    );

    server.on("/drive", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
      [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
        Serial.println("/drive");

        sscanf(make_string(data_website, len), "%" SCNd16 ";%" SCNd16 ";%" SCNd16 ";%" SCNd16,
          &bb::webServer.driveRequest.x_pwm,
          &bb::webServer.driveRequest.a_pwm,
          &bb::webServer.driveRequest.p_pwm,
          &bb::webServer.driveRequest.duration
        );

        bb::webServer.timeOfLastCommand = millis();
        bb::webServer.lastCommand = Command::DRIVE;

        request->send(200);
      }
    );

    server.on("/drive_imu", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
      [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
        Serial.println("/drive_imu");

        sscanf(make_string(data_website, len), "%" SCNd16 ";%f;%" SCNd16 ";%" SCNd16,
          &bb::webServer.driveIMURequest.vx,
          &bb::webServer.driveIMURequest.va,
          &bb::webServer.driveIMURequest.p,
          &bb::webServer.driveIMURequest.duration
        );

        //Serial.println((const char*)data_website);
        //Serial.println(driveIMURequest.vx);
        //Serial.println(driveIMURequest.va);
        //Serial.println(driveIMURequest.p);
        //Serial.println(driveIMURequest.duration);

        bb::webServer.timeOfLastCommand = millis();
        bb::webServer.lastCommand = Command::DRIVE_IMU;

        request->send(200);
      }
    );

    server.on("/goto_point", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
      [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
        Serial.println("/goto_point");

        sscanf(make_string(data_website, len), "%lf;%lf;%" SCNd16,
          &bb::webServer.gotoPointRequest.target.x,
          &bb::webServer.gotoPointRequest.target.y,
          &bb::webServer.gotoPointRequest.p_pwm
        );

        /*
        Serial.print(gotoPointRequest.target.x);
        Serial.print(",");
        Serial.print(gotoPointRequest.target.y);
        Serial.print(",");
        Serial.print(gotoPointRequest.p_pwm);
        */

        bb::webServer.timeOfLastCommand = millis();
        bb::webServer.lastCommand = Command::GOTO;

        request->send(200);
      }
    );
  }


  String getStatusMotionString() const {
    if (bb::behavior.isMoving()) {
      return "moving";
    } else {
      return "stopped";
    }
  }

  String getImuStatusString() const {
    if (bb::imu.isConnected()) {
      return "ok";
    } else {
      return "failed to connect";
    }
  }
};

#endif // WEB_SERVER_H
