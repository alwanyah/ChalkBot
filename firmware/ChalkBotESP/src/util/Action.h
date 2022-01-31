#ifndef ACTION_H
#define ACTION_H

#include <exception>
#include <ArduinoJson.h>

class BadActionType : public std::exception {
public:
    virtual const char *what() const noexcept override {
        return "BadActionType";
    }
};

struct ActionDrive {
    double forward;
    double clockwise;
    double print;
    double duration;

    ActionDrive(double forward, double clockwise, double print, double duration)
        : forward(forward)
        , clockwise(clockwise)
        , print(print)
        , duration(duration)
    {}
};

struct ActionGoto {
    double north;
    double east;
    double print;

    ActionGoto(double north, double east, double print)
        : north(north)
        , east(east)
        , print(print)
    {}
};

class Action {
public:
    static constexpr size_t JSON_SIZE = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4);

    enum Type {
        ERROR,
        DRIVE,
        GOTO,
    };

    Action()
        : type(ERROR)
    {}

    Action(ActionDrive drive)
        : type(DRIVE)
        , drive(drive)
    {}

    Action(ActionGoto goto_)
        : type(GOTO)
        , goto_(goto_)
    {}

    Type getType() const {
        return type;
    }

    const ActionDrive &getDrive() const {
        expectType(DRIVE);
        return drive;
    }

    const ActionGoto &getGoto() const {
        expectType(GOTO);
        return goto_;
    }

    void serialize(JsonObject root) const {
        switch (type) {
            case DRIVE: {
                root["type"] = "drive";
                JsonObject properties = root.createNestedObject("properties");
                properties["forward"] = drive.forward;
                properties["clockwise"] = drive.clockwise;
                properties["print"] = drive.print;
                properties["duration"] = drive.duration;
                break;
            }

            case GOTO: {
                root["type"] = "goto";
                JsonObject properties = root.createNestedObject("properties");
                properties["north"] = goto_.north;
                properties["east"] = goto_.east;
                properties["print"] = goto_.print;
                break;
            }

            default:
                throw BadActionType();
        }
    }

    static Action deserialize(JsonObjectConst root) {
        // FIXME: check ranges (value >= -1.0 && value <= 1.0)

        String type = root["type"];
        JsonObjectConst properties = root["properties"];
        if (type == "drive") {
            double forward = properties["forward"] | NAN;
            double clockwise = properties["clockwise"] | NAN;
            double print = properties["print"] | NAN;
            double duration = properties["duration"] | NAN;
            if (
                forward >= -1.0 && forward <= 1.0
                && clockwise >= -1.0 && clockwise <= 1.0
                && print >= -1.0 && print <= 1.0
                && duration >= 0.0
            ) {
                return ActionDrive(forward, clockwise, forward, duration);
            }
        } else if (type == "goto") {
            double north = properties["north"] | NAN;
            double east = properties["east"] | NAN;
            double print = properties["print"] | NAN;
            if (!isnan(north) && !isnan(east) && print >= -1.0 && print <= 1.0) {
                return ActionGoto(north, east, print);
            }
        }
        return Action();
    }

private:
    void expectType(Type expected) const {
        if (type != expected) {
            throw BadActionType();
        }
    }

    Type type;
    union {
        ActionDrive drive;
        ActionGoto goto_;
    };
};

#endif
