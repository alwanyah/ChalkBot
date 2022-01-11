#ifndef GNSS_BB_H
#define GNSS_BB_H

#include <Arduino.h>

class GnssBB {
  friend class GnssClass;

  bool globalPositionValid = false;
  double latitude = 0.0; // deg
  double longitude = 0.0; // deg
  double heightAboveCenter = 0.0; // m
  double heightAboveSea = 0.0; // m
  double horizontalAccuracy = 0.0; // m
  double verticalAccuracy = 0.0; // m

  bool relativePositionValid = false;
  double north = 0.0; // m
  double east = 0.0; // m
  double down = 0.0; // m
  double northAccuracy = 0.0; // m
  double eastAccuracy = 0.0; // m
  double downAccuracy = 0.0; // m

  bool headingValid = false;
  double heading = 0.0; // deg
  double headingAccuracy = 0.0; // deg

  bool fix = false;
  bool correction = false;

  size_t ntripNmeaBytesSent = 0;
  size_t ntripRtcmByesReceived = 0;

  unsigned long updateDuration = 0; // ms

public:
  bool hasGlobalPosition() const {
    return globalPositionValid;
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


  bool hasRelativePosition() const {
    return relativePositionValid;
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


  bool hasHeading() const {
    return headingValid;
  }

  double getHeading() const {
    return heading;
  }

  double getHeadingAccuracy() const {
    return headingAccuracy;
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
