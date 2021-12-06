#include <Wire.h>
#include <WiFi.h>
#include "Gnss.h"
#include "GnssBlackboard.h"
#include "WifiSecrets.h"

void setup() {
  Serial.begin(115200); // esp32 uses 115200Bd for system messages
  while (!Serial);

  Wire.begin(); // setup I2C master
  //Wire.setClock(400000); // set Clock to 400kHz

  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (!Gnss.begin()) {
    for (;;);
  }
}

void loop() {
  static unsigned long last_packet = millis();
  static bool converter_init = false;
  if (Gnss.update()) {
    unsigned long t = millis();
    unsigned long ms = t - last_packet;
    Serial.print("ms = ");
    Serial.printf("%4lu", ms);
    Serial.print(", ");
    GnssBlackboard.printAll();
    last_packet = t;
  }
}
