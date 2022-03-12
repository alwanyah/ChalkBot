#ifndef GNSS_BB_H
#define GNSS_BB_H

#include <Arduino.h>

class GnssBB {
  friend class Gnss;

  bool initialized = false;
  bool ntripConnected = false;

  unsigned long globalTimestamp = 0; // millis()
  double latitude = 0.0; // deg
  double longitude = 0.0; // deg
  double heightAboveCenter = 0.0; // m
  double heightAboveSea = 0.0; // m
  double horizontalAccuracy = 0.0; // m
  double verticalAccuracy = 0.0; // m

  unsigned long relativeTimestamp = 0; // millis()
  double north = 0.0; // m
  double east = 0.0; // m
  double down = 0.0; // m
  double northAccuracy = 0.0; // m
  double eastAccuracy = 0.0; // m
  double downAccuracy = 0.0; // m

  unsigned long motionTimestamp = 0; // millis()
  double heading = 0.0; // deg
  double headingAccuracy = 0.0; // deg
  double speed = 0.0; // m/s
  double speedAccuracy = 0.0; // m/s

  int satellites = 0;
  bool fix = false;
  bool correction = false;

  size_t ntripNmeaBytesSent = 0;
  size_t ntripRtcmByesReceived = 0;

  unsigned long updateDuration = 0; // ms

public:
  bool isInitialized() const {
    return initialized;
  }

  bool isNtripConnected() const {
    return ntripConnected;
  }


  unsigned long getGlobalTimestamp() const {
    return globalTimestamp;
  }

  double getLatitude() const {
    return latitude;
  }

  double getLongitude() const {
    return longitude;
  }

  double getHeightAboveCenter() const {
    return heightAboveCenter;
  }

  double getHeightAboveSea() const {
    return heightAboveSea;
  }

  double getHorizontalAccuracy() const {
    return horizontalAccuracy;
  }

  double getHerticalAccuracy() const {
    return verticalAccuracy;
  }


  unsigned long getRelativeTimestamp() const {
    return relativeTimestamp;
  }

  double getNorth() const {
    return north;
  }

  double getEast() const {
    return east;
  }

  double getDown() const {
    return down;
  }

  double getNorthAccuracy() const {
    return northAccuracy;
  }

  double getEastAccuracy() const {
    return eastAccuracy;
  }

  double getDownAccuracy() const {
    return downAccuracy;
  }


  unsigned long getMotionTimestamp() const {
    return motionTimestamp;
  }

  double getHeading() const {
    return heading;
  }

  double getHeadingAccuracy() const {
    return headingAccuracy;
  }

  double getSpeed() const {
    return speed;
  }

  double getSpeedAccuracy() const {
    return speedAccuracy;
  }


  int getSatellites() const {
    return satellites;
  }

  bool hasFix() const {
    return fix;
  }

  bool hasCorrection() const {
    return correction;
  }


  size_t getNtripNmeaBytesSent() const {
    return ntripNmeaBytesSent;
  }

  size_t getNtripRtcmByesReceived() const {
    return ntripRtcmByesReceived;
  }

  unsigned long getUpdateDuration() const {
    return updateDuration;
  }
};

#endif
