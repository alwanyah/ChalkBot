#include "Logger.h"
#include "Imu.h"
#include "Gnss.h"
#include "Odometry.h"
#include "Net.h"
#include "WebServer.h"
#include "Behavior.h"

// The setup routine runs once when you press reset.
void setup()
{
  Serial.begin(115200);
  while (!Serial) {};

  Logger::attach_listener(Serial, {{ "all", Logger::INFO }});

  Wire.begin(); // setup I2C master
  //Wire.setClock(400000); // set Clock to 400kHz

  Net.begin();
  WebServer.begin();

  Imu.begin();
  // Gnss.begin();

  delay(1000); // FIXME
}

void loop()
{
  Imu.update();
  // Gnss.update();
  Odometry.update();
  Behavior.update();
}
