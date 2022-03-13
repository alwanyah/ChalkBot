#ifndef CONFIG_H
#define CONFIG_H

// Magic constants go here.

#include <Arduino.h>

// FIXME: einheitliche Variablennamen (alles groß oder klein)
// FIXME: sollten diese Konstanten alle in die einzelnen .cpp Dateien rein?

namespace config {
    namespace pins {
        constexpr uint8_t motorFrontLeftPwm = 32; // A7
        constexpr uint8_t motorFrontLeftDir = 15; // A8

        constexpr uint8_t motorFrontRightPwm = 16; // RX
        constexpr uint8_t motorFrontRightDir = 17; // TX

        constexpr uint8_t motorRearLeftPwm = 13; // A12 // FIXME: this is connected to the builtin LED, maybe move somewhere else
        constexpr uint8_t motorRearLeftDir = 12; // A11

        constexpr uint8_t motorRearRightPwm = 26; // A0/DAC2
        constexpr uint8_t motorRearRightDir = 25; // A1/DAC1

        constexpr uint8_t printPwm = 21;
        constexpr uint8_t printPin1 = 4; // A5 // FIXME: pin1 = forward, pin2 = backward?
        constexpr uint8_t printPin2 = 14; // A6

        constexpr uint8_t i2cData = 23; // SDA
        constexpr uint8_t i2cClock = 22; // SCL
    }

    namespace ports {
        constexpr uint16_t WEB_SERVER = 80; // TCP
        constexpr uint16_t LOG_SERVER = 8001; // TCP
        constexpr uint16_t ECHO_SERVER = 8002; // UDP
        constexpr uint16_t WEB_API = 8080; // TCP
    }

    namespace wifi {
        /* aus ChalkBotESP.ino:
            chalkWebServer.initAP("chalkbot", "a1b0a1b0a1");
            //chalkWebServer.init("LAMBARUM", "DBiru1Sd90M.");
            //chalkWebServer.init("NAONET", "a1b0a1b0a1");
        */
        constexpr char AP_SSID[] = "chalkbot";
        constexpr char AP_PASSPHRASE[] = "a1b0a1b0a1";
    }

    namespace odometry {
        // fixed parameters
        constexpr float wheelDiameter = 135.0f; // mm
        constexpr float maxRPM = 313.0f; // max RPM of the motors with PWM 255
        constexpr float wheelCircumference = wheelDiameter * PI;
        constexpr float maxVelocity = wheelCircumference * maxRPM / 60.0f; // mm/s

        // measured factor for the current chalkbot
        constexpr float correctionFactor = 4.0 / 3.0;

    }

    namespace fusion {
        // adjust our position if it is more than (factor * accuracy) away from GNSS position
        constexpr double MAX_DEVIATION_FACTOR = 3.0;

        // after adjusting our position it is (factor * accuracy) away from GNSS position
        constexpr double ADJUSTMENT_TARGET_FACTOR = 1.0;
    }

    namespace ntrip {
        constexpr char HOST[] = "www.sapos-be-ntrip.de";
        constexpr uint16_t PORT = 2101;
        constexpr char MOUNT_POINT[] = "VRS_3_4G_BE";

        // base64 saposbln426-1:gbm10-40hu
        // yes, this sent in plaintext over an unencrypted connection
        constexpr char AUTHORIZATION[] = "c2Fwb3NibG40MjYtMTpnYm0xMC00MGh1";
    }

    // multicast local scope (67.66 = "CB")
    constexpr uint8_t DISCOVERY_ADDRESS[4] = { 239, 255, 67, 66 };

    constexpr size_t ACTION_QUEUE_CAPACITY = 5;
}

#endif
