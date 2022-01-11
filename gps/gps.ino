#include <Wire.h>
#include <WiFi.h>
#include "Gnss.h"
#include "GnssBlackboard.h"
#include "Logger.h"
#include "WifiSecrets.h"

Logger logger("main");
WiFiServer logServer(8001);

void setup() {
  Serial.begin(115200); // esp32 uses 115200Bd for system messages
  while (!Serial);

  Logger::attach_listener(Serial, Logger::INFO, {{ "GNSS", Logger::DEBUG }});

  Wire.begin(); // setup I2C master
  //Wire.setClock(400000); // set Clock to 400kHz

  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  auto writer = logger.writer_info();
  writer.print("local ip = ");
  writer.print(WiFi.localIP());
  writer.finish();

  logServer.begin();

  if (!Gnss.begin()) {
    for (;;);
  }
}

void loop() {
  WiFiClient logClient = logServer.available();
  if (logClient) {
    Logger::attach_listener_managed(new auto(logClient), Logger::DEBUG);
  }
  Gnss.update();
}
