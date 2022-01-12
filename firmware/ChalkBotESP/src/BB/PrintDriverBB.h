#ifndef PRINT_DRIVER_BB_H
#define PRINT_DRIVER_BB_H

#include <Arduino.h>

class PrintDriverBB {
    friend class PrintDriver;

    int16_t speed = 0;

public:
    void setSpeed(int16_t newSpeed) {
        speed = newSpeed;
    }
};

#endif
