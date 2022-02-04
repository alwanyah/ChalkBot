#ifndef IMU_H
#define IMU_H

// Inertial measurement unit.

#include <Arduino.h>
#include <Adafruit_BNO055.h>

class Imu {
    Adafruit_BNO055 bno;

public:
    Imu() : bno(55) {}

    bool begin();
    void update();
};

#endif
