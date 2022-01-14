#include "src/Config.h"
#include "src/modules/ChalkbotMotorDriver.h"
#include "src/modules/PrintDriver.h"
#include "src/modules/ChalkbotMotorController.h"
#include "src/modules/Imu.h"
#include "src/modules/Gnss.h"
#include "src/modules/Odometry.h"
#include "src/modules/Net.h"
#include "src/modules/WebServer.h"
#include "src/modules/Behavior.h"
#include "src/util/Logger.h"

static ChalkbotMotorDriver chalkbotMotorDriver(
  { config::pins::motorFrontLeftPwm, config::pins::motorFrontLeftDir },
  { config::pins::motorFrontRightPwm, config::pins::motorFrontRightDir },
  { config::pins::motorRearLeftPwm, config::pins::motorRearLeftDir },
  { config::pins::motorRearRightPwm, config::pins::motorRearRightDir }
);

static PrintDriver printDriver(
  config::pins::printPwm,
  config::pins::printPin1,
  config::pins::printPin2
);

static ChalkbotMotorController chalkbotMotorController;
static Imu imu_; // name conflicts with namespace imu from <Adafruit_BNO055.h>
static Gnss gnss;
static Odometry odometry;
static Net net;
static WebServer webServer;
static Behavior behavior;

static WiFiServer logServer(8001);


// The setup routine runs once when you press reset.
void setup()
{
  Serial.begin(115200);
  while (!Serial) {};

  Logger::attach_listener(Serial, {{ "all", Logger::INFO });

  Wire.begin(); // setup I2C master
  //Wire.setClock(400000); // set Clock to 400kHz

  net.begin();
  webServer.begin();

  imu_.begin();
  gnss.begin();

  logServer.begin();

  delay(1000); // FIXME
}

void loop()
{
  WiFiClient logClient = logServer.available();
  if (logClient) {
    Logger::attach_listener_managed(new auto(logClient), {{ "all", Logger::DEBUG }});
  }

  net.update();
  imu_.update();
  gnss.update();
  odometry.update();
  behavior.update();
  chalkbotMotorController.update();
  chalkbotMotorDriver.update();
  printDriver.update();
}
