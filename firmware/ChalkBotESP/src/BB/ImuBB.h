#ifndef IMU_BB_H
#define IMU_BB_H

#include <Arduino.h>

class ImuBB {
    friend class Imu;

    bool connected = false;
    double orientation = 0.0; // FIXME: unit. deg? rad?

public:
    bool isConnected() const {
        return connected;
    }

    double getOrientation() const {
        return orientation;
    }
};

#endif
