#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include "GnssBlackboard.h"

#define INIT_ATTEMPTS 5

class Gnss {
  SFE_UBLOX_GNSS base;

public:
  // Call Wire.begin() before this function.
  bool begin() {
    for (int attempt = 1; attempt < INIT_ATTEMPTS; ++attempt) {
      if (attempt == 2) {
        Serial.println("Resetting GNSS sensor...");
        base.factoryReset();
        delay(1000);
      }
      
      Serial.print("Initializing GNSS...");
      if (attempt == 1) {
        Serial.println();
      } else {
        Serial.print(" (attempt ");
        Serial.print(attempt);
        Serial.print("/");
        Serial.print(INIT_ATTEMPTS);
        Serial.println(")");
      }

      if (!base.begin()) {
        Serial.println("GNSS sensor not found.");
        continue;
      }

      // only output UBX (u-blox propritary) packets, no NMEA
      if (!base.setI2COutput(COM_TYPE_UBX)) {
        Serial.println("Failed to set output format.");
        continue;
      }

      // set update frequency
      if (!base.setNavigationFrequency(10)) {
        Serial.println("Failed to set navigation frequency.");
        continue;
      }

      // enable non-blocking mode
      // PVT = position/velocity/time
      if (!base.setAutoPVT(true)) {
        Serial.println("Failed to enable non-blocking mode.");
        continue;
      }
      
      // save config to flash and BBR (battery backed RAM)
      if (!base.saveConfiguration()) {
        Serial.println("Failed to enable save configuration.");
        continue;
      }

      Serial.println("GNSS initialized.");
      return true;
    }

    Serial.println("failed to initialize GNSS.");
    return false;
  }

  bool update(GnssBlackboard &blackboard) {
    if (!base.getPVT()) {
      return false;
    }
    blackboard.siv = base.getSIV();
    blackboard.longitude = base.getLongitude() / 1e7;
    blackboard.latitude = base.getLatitude() / 1e7;
    return true;
  }
};
