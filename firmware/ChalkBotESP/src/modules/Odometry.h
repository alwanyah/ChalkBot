#ifndef ODOMETRY_H
#define ODOMETRY_H

#include <Arduino.h>

class Odometry {
    unsigned long lastUpdateTime = 0;

public:
    void update();
};

#endif
