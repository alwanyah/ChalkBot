class GnssBlackboardClass {
  friend class GnssClass;
  
  bool globalPositionValid = false;
  double latitude = 0.0;
  double longitude = 0.0;
  double heightAboveCenter = 0.0;
  double heightAboveSea = 0.0;
  double horizontalAccuracy = 0.0;
  double verticalAccuracy = 0.0;

  bool relativePositionValid = false;
  double north = 0.0;
  double east = 0.0;
  double down = 0.0;
  double northAccuracy = 0.0;
  double eastAccuracy = 0.0;
  double downAccuracy = 0.0;

  bool headingValid = false;
  double heading = 0.0;
  double headingAccuracy = 0.0;

  bool fix = false;
  bool correction = false;

  size_t ntripNmeaBytesSent = 0;
  size_t ntripRtcmByesReceived = 0;

public:
  double getLatitude() const {
    return latitude;
  }

  double getLongitude() const {
    return longitude;
  }

  // TODO: getters

  void printAll() const {
    Serial.print("ntrip send = ");
    Serial.printf("%4zu", ntripNmeaBytesSent);
    Serial.print(" recv = ");
    Serial.printf("%4zu", ntripRtcmByesReceived);
    if (globalPositionValid) {
      Serial.print(", lat = ");
      Serial.print(latitude, 9);
      Serial.print(", lon = ");
      Serial.print(longitude, 9);
      Serial.print(", acc = ");
      Serial.print(horizontalAccuracy, 4);
    }
    if (relativePositionValid) {
      Serial.print(", N = ");
      Serial.print(north, 4);
      Serial.print("+-");
      Serial.print(northAccuracy, 4);
      Serial.print(", E = ");
      Serial.print(east, 4);
      Serial.print("+-");
      Serial.print(eastAccuracy, 4);
    }
    Serial.print(", fix = ");
    Serial.print(fix);
    Serial.print(", cor = ");
    Serial.print(correction);
    Serial.println();
  }
};

extern GnssBlackboardClass GnssBlackboard;
