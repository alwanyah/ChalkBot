#include "Net.h"
#include "BB.h"
#include "Logger.h"
#include "Config.h"

#include <WiFi.h>

NetClass Net;

static Logger logger("net");

// extracted from ChalkWebServerAsync.h
// FIXME: should be replaced
bool NetClass::initAP(const char *ssid, const char *password) 
{
    logger.log_info("Initialising WiFi as access point...");

    // Set Static IP address
    IPAddress local_IP(10, 0, 4, 99);
    // Set your Gateway IP address
    IPAddress gateway(10, 0, 4, 1);
    IPAddress subnet(255, 255, 0, 0);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    delay(1000);
        
    // Configures static IP address
    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
        logger.log_error("AP Failed to configure");
        return false;
    } else {
        delay(1000); // FIXME
    }

    bb::net.localAddress = WiFi.softAPIP();

    auto writer = logger.writer_info();
    writer.print("Connected. Local address: ");
    writer.print(bb::net.localAddress);
    writer.finish();
    return true;
}

// extracted from ChalkWebServerAsync.h
// FIXME: should be replaced
bool NetClass::init(const char *ssid, const char *password) 
{
    logger.log_info("Initialising WiFi as station...");

    WiFi.mode(WIFI_AP_STA);
    //WiFi.mode(WIFI_STA);
    {
        auto writer = logger.writer_info();
        writer.print("connecting to: ");
        writer.print(WiFi.softAPIP());
        writer.finish();
    }
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); // FIXME
        Serial.print(".");
    }

    /*
    // connect to existing wifi
    // Set Static IP address
    IPAddress local_IP(10, 0, 4, 99);
    // Set your Gateway IP address
    IPAddress gateway(10, 0, 4, 1);
    IPAddress subnet(255, 255, 0, 0);
    if (!WiFi.config(local_IP, gateway, subnet)) {
        logger.log_error("STA Failed to configure");
    }
    */

    bb::net.localAddress = WiFi.localIP();

    //WiFi.setHostname("chalkbot");

    auto writer = logger.writer_info();
    writer.print("Connected. Local address: ");
    writer.print(bb::net.localAddress);
    writer.finish();
    return true;
}

bool NetClass::begin() {
    return initAP(config::wifi::AP_SSID, config::wifi::AP_PASSPHRASE);
}

void NetClass::update() {
    // TODO
}
