#ifndef GNSS_H
#define GNSS_H

// Global Navigation Sattellite System.

#include <Arduino.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <WiFiClient.h>

class Gnss {
  // NOTE: if we ever destroy this, base must be ordered before ntripClient (see base.setNMEAOutputPort(ntripClient))
  SFE_UBLOX_GNSS base;
  WiFiClient ntripClient;
  bool ntripConnected = false;

public:
  // Call Wire.begin() before this function.
  bool begin();
  bool update();

private:
  void updateHPPOSLLH(UBX_NAV_HPPOSLLH_data_t *data);
  void updateRELPOSNED(UBX_NAV_RELPOSNED_data_t *data);

  bool ntripConnect();
  void ntripDisconnect();
  size_t ntripReceiveRTCM();
};

#endif
