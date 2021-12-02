#include <Wire.h>
#include "Gnss.h"

Gnss gnss;
GnssBlackboard gnssBlackboard;

void setup() {
  Serial.begin(115200); // esp32 uses 115200Bd for system messages
  while (!Serial);

  Wire.begin(); // setup I2C master
  //Wire.setClock(400000); // set Clock to 400kHz

  if (!gnss.begin()) {
    for (;;);
  }
}

void loop() {
  static auto last_packet = millis();
  if (gnss.update(gnssBlackboard)) {

    Serial.print("time = ");
    Serial.print(millis() - last_packet);
    Serial.print(", siv = ");
    Serial.print(gnssBlackboard.getSatellitesInView());
    Serial.print(", lon = ");
    Serial.print(gnssBlackboard.getLongitude(), 7);
    Serial.print(", lat = ");
    Serial.println(gnssBlackboard.getLatitude(), 7);

    last_packet = millis();
  }
}
