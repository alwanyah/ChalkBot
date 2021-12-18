# ChalkBot Firmware

The proram running in an embeaded computer and providing low-level 
control for the ChalkBot.

## ESP32

Currently we use an ESP32 based computer to control the robot. 
How to set up at ESP32 for Arduino can be found in the following tutorials.

* Installing the ESP32 Board in Arduino IDE (Windows, Mac OS X, Linux)
  https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/

* ESP32 Tutorials
  https://randomnerdtutorials.com/getting-started-with-esp32/

* Adafruit ESP32 Feather Board
  https://www.adafruit.com/product/3405

## IMU Sensor - BNO055

* https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor

## WIFI and WebServer 
You need to install those libraries for the web server to work:

* https://github.com/me-no-dev/AsyncTCP
* https://github.com/me-no-dev/ESPAsyncWebServer