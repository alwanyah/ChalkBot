#ifndef NET_H
#define NET_H

#include <Arduino.h>

class Net {
public:
    // FIXME: remove these, use begin
    bool initAP(const char *ssid, const char *password);
    bool init(const char *ssid, const char *password);

    bool begin();
    void update();
};

#endif
