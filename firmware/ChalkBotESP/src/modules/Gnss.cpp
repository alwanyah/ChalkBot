#include "Gnss.h"
#include "../BB.h"
#include "../Config.h"
#include "../util/Logger.h"

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
    if (!base.setAutoPVT(true)) {
      logger.log_warn("Failed to enable non-blocking mode for PVT.");
      continue;
    }

    // // HPPOSLLH = high precision position latitude/longitude/height
    // if (!base.setAutoHPPOSLLH(true)) {
    //   logger.log_warn("Failed to enable non-blocking mode for HPPOSLLH.");
    //   continue;
    // }

    // RELPOSNED = relative position north/east/down
    if (!base.setAutoRELPOSNED(true)) {
      logger.log_warn("Failed to enable non-blocking mode for RELPOSNED.");
      continue;
    }

    // // VELNED = velocity north/east/down
    // if (!base.setAutoNAVVELNED(true)) {
    //   logger.log_warn("Failed to enable non-blocking mode for VELNED.");
    //   continue;
    // }

    // save config to flash and BBR (battery backed RAM)
    if (!base.saveConfiguration()) {
      logger.log_warn("Failed to enable save configuration.");
      continue;
    }

    // base.setProcessNMEAMask(SFE_UBLOX_FILTER_NMEA_GGA);

    base.setNMEAOutputPort(*this);

    bb::gnss.initialized = true;

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
    bb::gnss.ntripNmeaBytesSent = ntripSendNMEA();
    bb::gnss.ntripRtcmByesReceived = ntripReceiveRTCM();
    if (bb::gnss.ntripNmeaBytesSent != 0 || bb::gnss.ntripRtcmByesReceived != 0) {
      hasUpdate = true;
    }
  } else {
    bb::gnss.ntripNmeaBytesSent = 0;
    bb::gnss.ntripRtcmByesReceived = 0;
  }

  if (base.packetUBXNAVPVT != nullptr && base.packetUBXNAVPVT->moduleQueried.moduleQueried1.all != 0) {
    updatePVT(&base.packetUBXNAVPVT->data);
    base.flushPVT();
    hasUpdate = true;
  }

  // if (base.packetUBXNAVHPPOSLLH != nullptr && base.packetUBXNAVHPPOSLLH->moduleQueried.moduleQueried.all != 0) {
  //   updateHPPOSLLH(&base.packetUBXNAVHPPOSLLH->data);
  //   base.flushHPPOSLLH();
  //   hasUpdate = true;
  // }

  if (base.packetUBXNAVRELPOSNED != nullptr && base.packetUBXNAVRELPOSNED->moduleQueried.moduleQueried.all != 0) {
    updateRELPOSNED(&base.packetUBXNAVRELPOSNED->data);
    base.flushNAVRELPOSNED();
    hasUpdate = true;
  }

  // if (base.packetUBXNAVVELNED != nullptr && base.packetUBXNAVVELNED->moduleQueried.moduleQueried.all != 0) {
  //   updateVELNED(&base.packetUBXNAVVELNED->data);
  //   base.flushNAVRELPOSNED();
  //   hasUpdate = true;
  // }

  if (hasUpdate) {
    bb::gnss.updateDuration = millis() - updateBegin;
    logData();
  }

  return hasUpdate;
}

void Gnss::updatePVT(UBX_NAV_PVT_data_t *data) {
  if (!data->flags3.bits.invalidLlh) {
    bb::gnss.latitude = data->lat / 1e7;
    bb::gnss.longitude = data->lon / 1e7;
    bb::gnss.heightAboveCenter = data->height / 1e3;
    bb::gnss.heightAboveSea = data->hMSL / 1e3;
    bb::gnss.horizontalAccuracy = data->hAcc / 1e4;
    bb::gnss.verticalAccuracy = data->vAcc / 1e4;
    bb::gnss.globalTimestamp = millis();
  }

  bb::gnss.northVelocity = data->velN / 1e3;
  bb::gnss.eastVelocity = data->velE / 1e3;
  bb::gnss.downVelocity = data->velD / 1e3;
  bb::gnss.speed = data->gSpeed / 1e3;
  bb::gnss.speedAccuracy = data->sAcc / 1e3;
  bb::gnss.heading = data->headMot / 1e5;
  bb::gnss.headingAccuracy = data->headAcc / 1e5;
  bb::gnss.motionTimestamp = millis();

  bb::gnss.satellites = data->numSV;
}

// currently unused
void Gnss::updateHPPOSLLH(UBX_NAV_HPPOSLLH_data_t *data) {
  if (!data->flags.bits.invalidLlh) {
    bb::gnss.latitude = data->lat / 1e7 + data->latHp / 1e9;
    bb::gnss.longitude = data->lon / 1e7 + data->lonHp / 1e9;
    bb::gnss.heightAboveCenter = data->height / 1e3 + data->heightHp / 1e4;
    bb::gnss.heightAboveSea = data->hMSL / 1e3 + data->hMSLHp / 1e4;
    bb::gnss.horizontalAccuracy = data->hAcc / 1e4;
    bb::gnss.verticalAccuracy = data->vAcc / 1e4;
    bb::gnss.globalTimestamp = millis();
  }
}

void Gnss::updateRELPOSNED(UBX_NAV_RELPOSNED_data_t *data) {
  if (data->flags.bits.relPosValid) {
    bb::gnss.north = data->relPosN / 1e2 + data->relPosHPN / 1e4;
    bb::gnss.east = data->relPosE / 1e2 + data->relPosHPE / 1e4;
    bb::gnss.down = data->relPosD / 1e2 + data->relPosHPD / 1e4;
    bb::gnss.northAccuracy = data->accN / 1e4;
    bb::gnss.eastAccuracy = data->accE / 1e4;
    bb::gnss.downAccuracy = data->accD / 1e4;
    bb::gnss.relativeTimestamp = millis();
  }

  bb::gnss.fix = data->flags.bits.gnssFixOK;
  bb::gnss.correction = data->flags.bits.diffSoln;
}

void Gnss::updateVELNED(UBX_NAV_VELNED_data_t *data) {
  bb::gnss.northVelocity = data->velN / 1e2;
  bb::gnss.eastVelocity = data->velE / 1e2;
  bb::gnss.downVelocity = data->velD / 1e2;
  bb::gnss.speed = data->gSpeed / 1e2;
  bb::gnss.speedAccuracy = data->sAcc / 1e2;
  bb::gnss.heading = data->heading / 1e5;
  bb::gnss.headingAccuracy = data->cAcc / 1e5;
  bb::gnss.motionTimestamp = millis();
}

bool Gnss::ntripConnect() {
  static unsigned long last_failed_attempt = 0;
  if (bb::gnss.ntripConnected) {
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
  nmea_buffer_head = 0;
  nmea_buffer_tail = 0;
  nmea_need_sync = true;
  bb::gnss.ntripConnected = true;
  logger.log_info("Ntrip connected.");
  return true;
}

void Gnss::ntripDisconnect() {
  ntripClient.stop();
  bb::gnss.ntripConnected = false;
  logger.log_warn("Ntrip disconnected.");
}

size_t Gnss::ntripSendNMEA() {
  size_t nmea_size = nmea_buffer_tail - nmea_buffer_head;
  if (nmea_size == 0) {
    return 0;
  }
  int written = ntripClient.write(&nmea_buffer[nmea_buffer_head], nmea_size);
  if (written < 0) {
    ntripDisconnect();
    return 0;
  }
  nmea_buffer_head += written;
  return written;
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

size_t Gnss::write(uint8_t incoming) {
  if (!bb::gnss.ntripConnected) {
    return 1;
  }
  if (nmea_need_sync && incoming != '$') {
    return 1;
  }
  nmea_need_sync = false;
  if (nmea_buffer_tail == sizeof nmea_buffer) {
    if (nmea_buffer_head == 0) {
      logger.log_warn("NMEA buffer full!");
      nmea_need_sync = true;
      return 0;
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
  return 1;
}

static void logData() {
  auto writer = logger.writer_debug();
  writer.print("ms = ");
  writer.printf("%3lu", bb::gnss.getUpdateDuration());
  writer.print(", ntrip send = ");
  writer.printf("%4zu", bb::gnss.getNtripNmeaBytesSent());
  writer.print(" recv = ");
  writer.printf("%4zu", bb::gnss.getNtripRtcmByesReceived());
  writer.print(", siv = ");
  writer.printf("%2d", bb::gnss.getSatellites());

  writer.print(", lat = ");
  writer.print(bb::gnss.getLatitude(), 9);
  writer.print(", lon = ");
  writer.print(bb::gnss.getLongitude(), 9);
  writer.print(", acc = ");
  writer.print(bb::gnss.getHorizontalAccuracy(), 4);

  writer.print(", N = ");
  writer.print(bb::gnss.getNorth(), 4);
  writer.print("+-");
  writer.print(bb::gnss.getNorthAccuracy(), 4);
  writer.print(", E = ");
  writer.print(bb::gnss.getEast(), 4);
  writer.print("+-");
  writer.print(bb::gnss.getEastAccuracy(), 4);

  writer.print(", head = ");
  writer.print(bb::gnss.getHeading(), 5);
  writer.print("+-");
  writer.print(bb::gnss.getHeadingAccuracy(), 5);

  writer.print(", speed = ");
  writer.print(bb::gnss.getSpeed(), 3);
  writer.print("+-");
  writer.print(bb::gnss.getSpeedAccuracy(), 3);

  writer.print(", fix = ");
  writer.print(bb::gnss.hasFix());
  writer.print(", cor = ");
  writer.print(bb::gnss.hasCorrection());
  writer.finish();
}
