#include "Gnss.h"
#include "../BB.h"
#include "../Config.h"
#include "../util/Logger.h"

class NullStream : public Stream {
  virtual int available() override {
    return 0;
  };
  virtual int read() override {
    return 0;
  };
  virtual int peek() override {
    return 0;
  };
  virtual void flush() override {};
  virtual size_t write(uint8_t) override {
    return 0;
  }
};

static NullStream nullStream;

static constexpr int INIT_ATTEMPTS = 5;
static constexpr size_t RTCM_BUFFER_SIZE = 2048;

static constexpr unsigned long NTRIP_RETRY_MILLIS = 2000;

static Logger logger("GNSS");

static void logData();

bool Gnss::begin() {
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

bool Gnss::update() {
  unsigned long updateBegin = millis();

  if (!base.checkUblox()) {
    return false;
  }

  bool hasUpdate = false;

  if (ntripConnect()) {
    bb::gnss.ntripRtcmByesReceived = ntripReceiveRTCM();
    if (bb::gnss.ntripNmeaBytesSent != 0 || bb::gnss.ntripRtcmByesReceived != 0) {
      hasUpdate = true;
    }
  } else {
    bb::gnss.ntripNmeaBytesSent = 0;
    bb::gnss.ntripRtcmByesReceived = 0;
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
    bb::gnss.updateDuration = millis() - updateBegin;
    logData();
  }

  return hasUpdate;
}

void Gnss::updateHPPOSLLH(UBX_NAV_HPPOSLLH_data_t *data) {
  if (data->flags.bits.invalidLlh) {
    bb::gnss.globalPositionValid = false;
    return;
  }
  bb::gnss.globalPositionValid = data->hAcc != UINT32_MAX;
  bb::gnss.latitude = data->lat / 1e7 + data->latHp / 1e9;
  bb::gnss.longitude = data->lon / 1e7 + data->lonHp / 1e9;
  bb::gnss.heightAboveCenter = data->height / 1e3 + data->heightHp / 1e4;
  bb::gnss.heightAboveSea = data->hMSL / 1e3 + data->hMSLHp / 1e4;
  bb::gnss.horizontalAccuracy = data->hAcc / 1e4;
  bb::gnss.verticalAccuracy = data->vAcc / 1e4;
}

void Gnss::updateRELPOSNED(UBX_NAV_RELPOSNED_data_t *data) {
  bb::gnss.relativePositionValid = data->flags.bits.relPosValid;
  if (bb::gnss.relativePositionValid) {
    bb::gnss.relativePositionValid = true;
    bb::gnss.north = data->relPosN / 1e2 + data->relPosHPN / 1e4;
    bb::gnss.east = data->relPosE / 1e2 + data->relPosHPE / 1e4;
    bb::gnss.down = data->relPosD / 1e2 + data->relPosHPD / 1e4;
    bb::gnss.northAccuracy = data->accN / 1e4;
    bb::gnss.eastAccuracy = data->accE / 1e4;
    bb::gnss.downAccuracy = data->accD / 1e4;
  }

  bb::gnss.headingValid = data->flags.bits.relPosHeadingValid;
  if (bb::gnss.headingValid) {
    bb::gnss.heading = data->relPosHeading / 1e5;
  }

  bb::gnss.fix = data->flags.bits.gnssFixOK;
  bb::gnss.correction = data->flags.bits.diffSoln;
}

bool Gnss::ntripConnect() {
  static unsigned long last_failed_attempt = 0;
  if (ntripConnected) {
    return true;
  }
  if (millis() - last_failed_attempt < NTRIP_RETRY_MILLIS) {
    return false;
  }
  if (!ntripClient.connect(config::ntrip::HOST, config::ntrip::PORT)) {
    logger.log_info("Failed to connect to Ntrip.");
    last_failed_attempt = millis();
    return false;
  }
  String request = String("GET /") + config::ntrip::MOUNT_POINT + " HTTP/1.1\r\n"
    "Host: " + config::ntrip::HOST + "\r\n"
    "User-Agent: ChalkBot/1.0\r\n"
    "Connection: close\r\n" // <- not actually, this is Ntrip, not HTTP
    "Authorization: Basic " + config::ntrip::AUTHORIZATION + "\r\n"
    "\r\n";

  ntripClient.print(request);
  String response = ntripClient.readStringUntil('\n');
  if (response != "ICY 200 OK\r" || !ntripClient.find("\r\n")) {
    auto writer = logger.writer_info();
    writer.print("Ntrip response: ");
    writer.print(response);
    writer.finish();

    ntripClient.stop();
    last_failed_attempt = millis();
    return false;
  }
  logger.log_info("Ntrip connected.");
  base.setNMEAOutputPort(ntripClient);
  return true;
}

void Gnss::ntripDisconnect() {
  base.setNMEAOutputPort(nullStream);
  ntripClient.stop();
  ntripConnected = false;
  logger.log_warn("Ntrip disconnected.");
}

size_t Gnss::ntripReceiveRTCM() {
  size_t total_read = 0;
  uint8_t buffer[RTCM_BUFFER_SIZE];
  int size;
  do {
    size = ntripClient.read(buffer, sizeof buffer);
    if (size < 0) {
      ntripDisconnect();
      break;
    } else if (size > 0) {
      if (size == sizeof buffer) {
        logger.log_warn("RTCM buffer full.");
      }
      total_read += size;
      if (!base.pushRawData(buffer, size)) {
        logger.log_error("Failed to send RTCM data.");
      }
    }
  } while (size == sizeof buffer);
  return total_read;
}

static void logData() {
  auto writer = logger.writer_debug();
  writer.print("ms = ");
  writer.printf("%3lu", bb::gnss.getUpdateDuration());
  writer.print(", ntrip send = ");
  writer.printf("%4zu", bb::gnss.getNtripNmeaBytesSent());
  writer.print(" recv = ");
  writer.printf("%4zu", bb::gnss.getNtripRtcmByesReceived());
  if (bb::gnss.hasGlobalPosition()) {
    writer.print(", lat = ");
    writer.print(bb::gnss.getLatitude(), 9);
    writer.print(", lon = ");
    writer.print(bb::gnss.getLongitude(), 9);
    writer.print(", acc = ");
    writer.print(bb::gnss.getHorizontalAccuracy(), 4);
  }
  if (bb::gnss.hasRelativePosition()) {
    writer.print(", N = ");
    writer.print(bb::gnss.getNorth(), 4);
    writer.print("+-");
    writer.print(bb::gnss.getNorthAccuracy(), 4);
    writer.print(", E = ");
    writer.print(bb::gnss.getEast(), 4);
    writer.print("+-");
    writer.print(bb::gnss.getEastAccuracy(), 4);
  }
  if (bb::gnss.hasHeading()) {
    writer.print(", head = ");
    writer.print(bb::gnss.getHeading(), 4);
    writer.print("+-");
    writer.print(bb::gnss.getHeadingAccuracy(), 4);
  }
  writer.print(", fix = ");
  writer.print(bb::gnss.hasFix());
  writer.print(", cor = ");
  writer.print(bb::gnss.hasCorrection());
  writer.finish();
}
