#include "Gnss.h"
#include "GnssBlackboard.h"

#include <WiFi.h>

static constexpr int INIT_ATTEMPTS = 5;
static constexpr size_t NMEA_BUFFER_SIZE = 1024;
static constexpr size_t RTCM_BUFFER_SIZE = 2048;

static constexpr const char *NTRIP_HOST = "www.sapos-be-ntrip.de";
static constexpr uint16_t NTRIP_PORT = 2101;
static constexpr const char *NTRIP_MOUNT_POINT = "VRS_3_4G_BE";
static constexpr unsigned long NTRIP_RETRY_MILLIS = 2000;

// base64 saposbln426-1:gbm10-40hu
// yes, this sent in plaintext over an unencrypted connection
static constexpr const char *NTRIP_AUTHORIZATION = "c2Fwb3NibG40MjYtMTpnYm0xMC00MGh1";

GnssBlackboardClass GnssBlackboard;
GnssClass Gnss;

static WiFiClient ntrip_client;
static bool ntrip_connected = false;
static char nmea_buffer[NMEA_BUFFER_SIZE];
static size_t nmea_buffer_head = 0;
static size_t nmea_buffer_tail = 0;
static bool nmea_need_sync = true;

bool GnssClass::begin() {
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

    // output UBX (u-blox propritary) packets and NMEA for sending to Ntrip
    if (!base.setI2COutput(COM_TYPE_UBX | COM_TYPE_NMEA)) {
      Serial.println("Failed to set output format.");
      continue;
    }

    // input UBX (control) and RTCM (correction data)
    if (!base.setPortInput(COM_PORT_I2C, COM_TYPE_UBX | COM_TYPE_RTCM3)) {
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
    // if (!base.setAutoPVT(true)) {
    //   Serial.println("Failed to enable non-blocking mode for PVT.");
    //   continue;
    // }

    // HPPOSLLH = high precision position latitude/longitude/height
    if (!base.setAutoHPPOSLLH(true)) {
      Serial.println("Failed to enable non-blocking mode for HPPOSLLH.");
      continue;
    }

    // RELPOSNED = relative position north/east/down
    if (!base.setAutoRELPOSNED(true)) {
      Serial.println("Failed to enable non-blocking mode for RELPOSNED.");
      continue;
    }
    
    // save config to flash and BBR (battery backed RAM)
    if (!base.saveConfiguration()) {
      Serial.println("Failed to enable save configuration.");
      continue;
    }

    // base.setProcessNMEAMask(SFE_UBLOX_FILTER_NMEA_GGA);

    Serial.println("GNSS initialized.");
    return true;
  }

  Serial.println("failed to initialize GNSS.");
  return false;
}

bool GnssClass::update() {
  if (!base.checkUblox()) {
    return false;
  }

  bool hasUpdate = false;

  if (ntripConnect()) {
    GnssBlackboard.ntripNmeaBytesSent = ntripSendNMEA();
    GnssBlackboard.ntripRtcmByesReceived = ntripReceiveRTCM();
    if (GnssBlackboard.ntripNmeaBytesSent != 0 || GnssBlackboard.ntripRtcmByesReceived != 0) {
      hasUpdate = true;
    }
  } else {
    GnssBlackboard.ntripNmeaBytesSent = 0;
    GnssBlackboard.ntripRtcmByesReceived = 0;
  }
  
  if (base.packetUBXNAVHPPOSLLH != NULL && base.packetUBXNAVHPPOSLLH->moduleQueried.moduleQueried.all != 0) {
    updateHPPOSLLH(&base.packetUBXNAVHPPOSLLH->data);
    base.flushHPPOSLLH();
    hasUpdate = true;
  }
  
  if (base.packetUBXNAVRELPOSNED != NULL && base.packetUBXNAVRELPOSNED->moduleQueried.moduleQueried.all != 0) {
    updateRELPOSNED(&base.packetUBXNAVRELPOSNED->data);
    base.flushNAVRELPOSNED();
    hasUpdate = true;
  }

  return hasUpdate;
}

void GnssClass::updateHPPOSLLH(UBX_NAV_HPPOSLLH_data_t *data) {
  if (data->flags.bits.invalidLlh) {
    GnssBlackboard.globalPositionValid = false;
    return;
  }
  GnssBlackboard.globalPositionValid = true;
  GnssBlackboard.latitude = data->lat / 1e7 + data->latHp / 1e9;
  GnssBlackboard.longitude = data->lon / 1e7 + data->lonHp / 1e9;
  GnssBlackboard.heightAboveCenter = data->height / 1e3 + data->heightHp / 1e4;
  GnssBlackboard.heightAboveSea = data->hMSL / 1e3 + data->hMSLHp / 1e4;
  GnssBlackboard.horizontalAccuracy = data->hAcc / 1e4;
  GnssBlackboard.verticalAccuracy = data->vAcc / 1e4;
}

void GnssClass::updateRELPOSNED(UBX_NAV_RELPOSNED_data_t *data) {
  GnssBlackboard.relativePositionValid = data->flags.bits.relPosValid;
  if (GnssBlackboard.relativePositionValid) {
    GnssBlackboard.relativePositionValid = true;
    GnssBlackboard.north = data->relPosN / 1e2 + data->relPosHPN / 1e4;
    GnssBlackboard.east = data->relPosE / 1e2 + data->relPosHPE / 1e4;
    GnssBlackboard.down = data->relPosD / 1e2 + data->relPosHPD / 1e4;
    GnssBlackboard.northAccuracy = data->accN / 1e4;
    GnssBlackboard.eastAccuracy = data->accE / 1e4;
    GnssBlackboard.downAccuracy = data->accD / 1e4;
  }

  GnssBlackboard.headingValid = data->flags.bits.relPosHeadingValid;
  if (GnssBlackboard.headingValid) {
    GnssBlackboard.heading = data->relPosHeading / 1e5;
  }

  GnssBlackboard.fix = data->flags.bits.gnssFixOK;
  GnssBlackboard.correction = data->flags.bits.diffSoln;
}

bool GnssClass::ntripConnect() {
  static unsigned long last_failed_attempt = 0;
  if (ntrip_connected) {
    return true;
  }
  if (millis() - last_failed_attempt < NTRIP_RETRY_MILLIS) {
    return false;
  }
  if (!ntrip_client.connect(NTRIP_HOST, NTRIP_PORT)) {
    Serial.println("Failed to connect to Ntrip.");
    last_failed_attempt = millis();
    return false;
  }
  String request = String("GET /") + NTRIP_MOUNT_POINT + " HTTP/1.1\r\n"
    "Host: " + NTRIP_HOST + "\r\n"
    "User-Agent: ChalkBot/1.0\r\n"
    "Connection: close\r\n" // <- not actually, this is Ntrip, not HTTP
    "Authorization: Basic " + NTRIP_AUTHORIZATION + "\r\n"
    "\r\n";
  
  ntrip_client.print(request);
  String response = ntrip_client.readStringUntil('\n');
  if (response != "ICY 200 OK\r" || !ntrip_client.find("\r\n")) {
    Serial.print("Ntrip response: ");
    Serial.println(response);
    ntrip_client.stop();
    last_failed_attempt = millis();
    return false;
  }
  nmea_buffer_head = 0;
  nmea_buffer_tail = 0;
  nmea_need_sync = true;
  ntrip_connected = true;
  Serial.println("Ntrip connected.");
  return true;
}

void GnssClass::ntripDisconnect() {
  ntrip_client.stop();
  ntrip_connected = false;
  Serial.println("Ntrip disconnected");
}

size_t GnssClass::ntripSendNMEA() {
  size_t nmea_size = nmea_buffer_tail - nmea_buffer_head;
  if (nmea_size == 0) {
    return 0;
  }
  int written = ntrip_client.write(&nmea_buffer[nmea_buffer_head], nmea_size);
  if (written < 0) {
    ntripDisconnect();
    return 0;
  }
  nmea_buffer_head += written;
  return written;
}

size_t GnssClass::ntripReceiveRTCM() {
  size_t total_read = 0;
  uint8_t buffer[RTCM_BUFFER_SIZE];
  int size;
  do {
    size = ntrip_client.read(buffer, sizeof buffer);
    if (size < 0) {
      ntripDisconnect();
      break;
    } else if (size > 0) {
      if (size == sizeof buffer) {
        Serial.println("RTCM buffer full!");
      }
      total_read += size;
      if (!Gnss.base.pushRawData(buffer, size)) {
        Serial.println("Failed to send RTCM data.");
      }
    }
  } while (size == sizeof buffer);
  return total_read;
}

void SFE_UBLOX_GNSS::processNMEA(char incoming) {
  if (!ntrip_connected) {
    return;
  }
  if (nmea_need_sync && incoming != '$') {
    return;
  }
  nmea_need_sync = false;
  if (nmea_buffer_tail == sizeof nmea_buffer) {
    if (nmea_buffer_head == 0) {
      Serial.println("NMEA buffer full!");
      nmea_need_sync = true;
      return;
    }
    size_t size = nmea_buffer_tail - nmea_buffer_head;
    if (size != 0) {
      memmove(&nmea_buffer[0], &nmea_buffer[nmea_buffer_head], size);
    }
    nmea_buffer_head = 0;
    nmea_buffer_tail = size;
  }
  nmea_buffer[nmea_buffer_tail] = incoming;
  ++nmea_buffer_tail;
}
