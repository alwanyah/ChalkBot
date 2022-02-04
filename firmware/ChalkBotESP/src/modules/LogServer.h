#ifndef LOG_SERVER_H
#define LOG_SERVER_H

#include <Arduino.h>
#include <WiFiServer.h>
#include "../Config.h"
#include "../util/Logger.h"

class LogServer {
    WiFiServer base;

public:
    LogServer() : base(config::ports::LOG_SERVER) {}

    void begin() {
        base.begin();
    }

    void update() {
        WiFiClient client = base.available();
        if (client) {
            Logger::attach_listener_managed(new WiFiClient(client), {{ "all", Logger::DEBUG }});
        }
    }
};

#endif
