#ifndef GNSS_H
#define GNSS_H

// Global Navigation Sattellite System.

#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

class GnssClass {
  SFE_UBLOX_GNSS base;

public:
  // Call Wire.begin() before this function.
  bool begin();

  bool update();

private:
  static void updateHPPOSLLH(UBX_NAV_HPPOSLLH_data_t *data);
  static void updateRELPOSNED(UBX_NAV_RELPOSNED_data_t *data);

  static bool ntripConnect();
  static void ntripDisconnect();
  static size_t ntripSendNMEA();
  static size_t ntripReceiveRTCM();
};

extern GnssClass Gnss;

#endif
