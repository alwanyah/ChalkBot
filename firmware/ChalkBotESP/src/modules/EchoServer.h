#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include "../Config.h"

static Logger logger("echo");

class EchoServer {
    static constexpr size_t BUFFER_SIZE = 512;

    WiFiUDP base;

public:
    void begin() {
        if (!base.beginMulticast(config::DISCOVERY_ADDRESS, config::ports::ECHO_SERVER)) {
            logger.log_error("Failed to start echo server.");
        }
    }

    void update() {
        int len = base.parsePacket();
        if (len <= 0) {
            return;
        }

        if (!base.beginPacket()) {
            logger.log_error("Failed prepare packet.");
            base.flush();
            return;
        }

        uint8_t buffer[BUFFER_SIZE];
        while ((len = base.read(buffer, BUFFER_SIZE)) > 0) {
            base.write(buffer, len);
        }

        if (!base.endPacket()) {
            logger.log_error("Failed send packet.");
        }
        base.flush();
    }
};

#endif
