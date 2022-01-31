#ifndef WEB_API_H
#define WEB_API_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "../Config.h"

class WebApi {
    AsyncWebServer server = AsyncWebServer(config::ports::WEB_API);

public:
    void begin();
};

#endif
