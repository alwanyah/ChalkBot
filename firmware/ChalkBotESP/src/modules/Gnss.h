#ifndef GNSS_H
#define GNSS_H

// Global Navigation Sattellite System.

#include <Arduino.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <WiFiClient.h>

class Gnss : private Stream {
  static constexpr int INIT_ATTEMPTS = 5;
  static constexpr size_t NMEA_BUFFER_SIZE = 1024;
  static constexpr size_t RTCM_BUFFER_SIZE = 2048;
  static constexpr unsigned long NTRIP_RETRY_MILLIS = 2000;

  SFE_UBLOX_GNSS base;
  WiFiClient ntripClient;

  uint8_t nmea_buffer[NMEA_BUFFER_SIZE];
  size_t nmea_buffer_head = 0;
  size_t nmea_buffer_tail = 0;
  bool nmea_need_sync = true;

public:
  // Call Wire.begin() before this function.
  bool begin();
  bool update();

private:
  void updatePVT(UBX_NAV_PVT_data_t *data);
  void updateHPPOSLLH(UBX_NAV_HPPOSLLH_data_t *data);
  void updateRELPOSNED(UBX_NAV_RELPOSNED_data_t *data);
  void updateVELNED(UBX_NAV_VELNED_data_t *data);

  bool ntripConnect();
  void ntripDisconnect();
  size_t ntripSendNMEA();
  size_t ntripReceiveRTCM();

  // Stream interface writes to NMEA buffer
  virtual int available() override { return 0; };
  virtual int read() override { return 0; };
  virtual int peek() override { return 0; };
  virtual void flush() override {};
  virtual size_t write(uint8_t) override;
};

#endif
