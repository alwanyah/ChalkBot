class GnssBlackboard {
  friend class Gnss;
  int siv = 0;
  double longitude = 0.0;
  double latitude = 0.0;

public:
  int getSatellitesInView() const {
    return siv;
  }

  double getLongitude() const {
    return longitude;
  }

  double getLatitude() const {
    return latitude;
  }
};
