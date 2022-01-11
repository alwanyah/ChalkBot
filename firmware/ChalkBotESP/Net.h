#ifndef NET_H
#define NET_H

#include <Arduino.h>

class NetClass {
public:
    // FIXME: remove these, use begin
    bool initAP(const char *ssid, const char *password);
    bool init(const char *ssid, const char *password);

    bool begin();
    void update();
};

extern NetClass Net;

#endif
