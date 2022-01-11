#ifndef IMU_H
#define IMU_H

// Inertial measurement unit.

#include <Adafruit_BNO055.h>

class ImuClass {
    Adafruit_BNO055 bno;
    
public:
    ImuClass() : bno(55) {}

    bool begin();
    void update();
};

extern ImuClass Imu;

#endif
