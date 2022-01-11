#ifndef CHALKBOT_MOTOR_BB_H
#define CHALKBOT_MOTOR_BB_H

#include <Arduino.h>

class ChalkbotMotorBB {
    friend class ChalkbotMotorClass;

    int16_t last_x = 0; // FIXME: rename to last_speed?
    int16_t last_rotation = 0;

public:
    int16_t getLastX() const {
        return last_x;
    }

    int16_t getLastRotation() const {
        return last_rotation;
    }
};

#endif
