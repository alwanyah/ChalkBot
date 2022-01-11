#ifndef CONFIG_H
#define CONFIG_H

// Magic constants go here.

#include <Arduino.h>

// FIXME: einheitliche Variablennamen (alles groß oder klein)
// FIXME: sollten diese Konstanten alle in die einzelnen .cpp Dateien rein?

namespace config {
    namespace pins {
        // FIXME: Pins fixen
        /* aus ChalkBotESP.ino:
            // mapping stm32 feather -> esp32 feather
            // PWM 5-> 4; 6-> 18; 9-> 19; 10-> 16
            // DIR A0->A12 ;A1->A11 ;A2->A10 ;A3->A9
            //PRINT: 23-> 5 (SCK); A4 -> A8; A5->A7

            // Configure the motor driver.
            ChalkbotMD chalkbot(
            { A7, A8 }, // motorFrontLeft : PWM =  4,  DIR = A12
            { 16, 17 }, // -motorFrontRight: PWM =  18, DIR = A11
            { A12,A11}, // motorRearLeft  : PWM =  19, DIR = A10
            { A0, A1 }  // motorRearRight : PWM =  16, DIR = A9
            );
        */
        constexpr uint8_t motorFrontLeftPwm = 0; // 4;
        constexpr uint8_t motorFrontLeftDir = 0; // A12;

        constexpr uint8_t motorFrontRightPwm = 0; // 18;
        constexpr uint8_t motorFrontRightDir = 0; // A11;

        constexpr uint8_t motorRearLeftPwm = 0; // 19;
        constexpr uint8_t motorRearLeftDir = 0; // A10;

        constexpr uint8_t motorRearRightPwm = 0; // 16;
        constexpr uint8_t motorRearRightDir = 0; // A9;

        /* aus ChalkBotESP.ino:
            // GPIO23 = SCK
        */
        // FIXME: Was bedeutet das ^ ?
        constexpr uint8_t printPwm = 21;
        constexpr uint8_t printPin1 = A5; // FIXME: pin1 = forward, pin2 = backward?
        constexpr uint8_t printPin2 = A6;
    }

    namespace wifi {
        /* aus ChalkBotESP.ino:
            chalkWebServer.initAP("chalkbot", "a1b0a1b0a1");
            //chalkWebServer.init("LAMBARUM", "DBiru1Sd90M.");
            //chalkWebServer.init("NAONET", "a1b0a1b0a1");
        */
        static constexpr char AP_SSID[] = "chalkbot";
        static constexpr char AP_PASSPHRASE[] = "a1b0a1b0a1";
    }

    namespace odometry {
        // FIXME: why float and not double?

        // fixed parameters
        static constexpr float wheelDiameter = 135.0f; // mm
        static constexpr float maxRPM = 313.0f; // max RPM of the motors with PWM 255
        static constexpr float wheelCircumference = wheelDiameter * PI;
        static constexpr float maxVelocity = wheelCircumference * maxRPM / 60.0f; // mm/s

        // measured factor for the current chalkbot
        static constexpr float correctionFactor = 4.0 / 3.0;

    }

    namespace ntrip {
        static constexpr char HOST[] = "www.sapos-be-ntrip.de";
        static constexpr uint16_t PORT = 2101;
        static constexpr char MOUNT_POINT[] = "VRS_3_4G_BE";

        // base64 saposbln426-1:gbm10-40hu
        // yes, this sent in plaintext over an unencrypted connection
        static constexpr char AUTHORIZATION[] = "c2Fwb3NibG40MjYtMTpnYm0xMC00MGh1";
    }
}

#endif
