#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include "../Config.h"

class EchoServer {
    WiFiUDP base;

public:
    void begin();
    void update();
};

#endif
