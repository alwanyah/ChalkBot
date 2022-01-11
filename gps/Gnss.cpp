#include "Gnss.h"
#include "GnssBlackboard.h"
#include "Logger.h"

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

static Logger logger("GNSS");

static WiFiClient ntrip_client;
static bool ntrip_connected = false;
static char nmea_buffer[NMEA_BUFFER_SIZE];
static size_t nmea_buffer_head = 0;
static size_t nmea_buffer_tail = 0;
static bool nmea_need_sync = true;

static void logData();

bool GnssClass::begin() {
  for (int attempt = 1; attempt < INIT_ATTEMPTS; ++attempt) {
    if (attempt == 2) {
      logger.log_warn("Resetting GNSS sensor.");
      base.factoryReset();
      delay(1000);
    }
    
    if (attempt == 1) {
      logger.log_info("Initializing GNSS...");
    } else {
      auto writer = logger.writer_info();
      writer.print("Initializing GNSS... (attempt ");
      writer.print(attempt);
      writer.print("/");
      writer.print(INIT_ATTEMPTS);
      writer.print(")");
      writer.finish();
    }

    if (!base.begin()) {
      logger.log_warn("GNSS sensor not found.");
      continue;
    }

    // output UBX (u-blox propritary) packets and NMEA for sending to Ntrip
    if (!base.setI2COutput(COM_TYPE_UBX | COM_TYPE_NMEA)) {
      logger.log_warn("Failed to set output format.");
      continue;
    }

    // input UBX (control) and RTCM (correction data)
    if (!base.setPortInput(COM_PORT_I2C, COM_TYPE_UBX | COM_TYPE_RTCM3)) {
      logger.log_warn("Failed to set output format.");
      continue;
    }

    // set update frequency
    if (!base.setNavigationFrequency(10)) {
      logger.log_warn("Failed to set navigation frequency.");
      continue;
    }

    // enable non-blocking mode
    // PVT = position/velocity/time
    // if (!base.setAutoPVT(true)) {
    //   logger.log_warn("Failed to enable non-blocking mode for PVT.");
    //   continue;
    // }

    // HPPOSLLH = high precision position latitude/longitude/height
    if (!base.setAutoHPPOSLLH(true)) {
      logger.log_warn("Failed to enable non-blocking mode for HPPOSLLH.");
      continue;
    }

    // RELPOSNED = relative position north/east/down
    if (!base.setAutoRELPOSNED(true)) {
      logger.log_warn("Failed to enable non-blocking mode for RELPOSNED.");
      continue;
    }
    
    // save config to flash and BBR (battery backed RAM)
    if (!base.saveConfiguration()) {
      logger.log_warn("Failed to enable save configuration.");
      continue;
    }

    // base.setProcessNMEAMask(SFE_UBLOX_FILTER_NMEA_GGA);

    logger.log_info("GNSS initialized.");
    return true;
  }

  logger.log_warn("failed to initialize GNSS.");
  return false;
}

bool GnssClass::update() {
  unsigned long updateBegin = millis();

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

  if (hasUpdate) {
    GnssBlackboard.updateDuration = millis() - updateBegin;
    logData();
  }

  return hasUpdate;
}

void GnssClass::updateHPPOSLLH(UBX_NAV_HPPOSLLH_data_t *data) {
  if (data->flags.bits.invalidLlh) {
    GnssBlackboard.globalPositionValid = false;
    return;
  }
  GnssBlackboard.globalPositionValid = data->hAcc != UINT32_MAX;
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
    logger.log_info("Failed to connect to Ntrip.");
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
    auto writer = logger.writer_info();
    writer.print("Ntrip response: ");
    writer.print(response);
    writer.finish();
    
    ntrip_client.stop();
    last_failed_attempt = millis();
    return false;
  }
  nmea_buffer_head = 0;
  nmea_buffer_tail = 0;
  nmea_need_sync = true;
  ntrip_connected = true;
  logger.log_info("Ntrip connected.");
  return true;
}

void GnssClass::ntripDisconnect() {
  ntrip_client.stop();
  ntrip_connected = false;
  logger.log_warn("Ntrip disconnected.");
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
        logger.log_warn("RTCM buffer full.");
      }
      total_read += size;
      if (!Gnss.base.pushRawData(buffer, size)) {
        logger.log_error("Failed to send RTCM data.");
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
      logger.log_warn("NMEA buffer full!");
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

static void logData() {
  auto writer = logger.writer_debug();
  writer.print("ms = ");
  writer.printf("%3zu", GnssBlackboard.getUpdateDuration());
  writer.print(", ntrip send = ");
  writer.printf("%4zu", GnssBlackboard.getNtripNmeaBytesSent());
  writer.print(" recv = ");
  writer.printf("%4zu", GnssBlackboard.getNtripRtcmByesReceived());
  if (GnssBlackboard.hasGlobalPosition()) {
    writer.print(", lat = ");
    writer.print(GnssBlackboard.getLatitude(), 9);
    writer.print(", lon = ");
    writer.print(GnssBlackboard.getLongitude(), 9);
    writer.print(", acc = ");
    writer.print(GnssBlackboard.getHorizontalAccuracy(), 4);
  }
  if (GnssBlackboard.hasRelativePosition()) {
    writer.print(", N = ");
    writer.print(GnssBlackboard.getNorth(), 4);
    writer.print("+-");
    writer.print(GnssBlackboard.getNorthAccuracy(), 4);
    writer.print(", E = ");
    writer.print(GnssBlackboard.getEast(), 4);
    writer.print("+-");
    writer.print(GnssBlackboard.getEastAccuracy(), 4);
  }
  if (GnssBlackboard.hasHeading()) {
    writer.print(", head = ");
    writer.print(GnssBlackboard.getHeading(), 4);
    writer.print("+-");
    writer.print(GnssBlackboard.getHeadingAccuracy(), 4);
  }
  writer.print(", fix = ");
  writer.print(GnssBlackboard.hasFix());
  writer.print(", cor = ");
  writer.print(GnssBlackboard.hasCorrection());
  writer.finish();
}
