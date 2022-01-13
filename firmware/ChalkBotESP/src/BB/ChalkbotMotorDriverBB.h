#ifndef CHALKBOT_MOTOR_DRIVER_BB_H
#define CHALKBOT_MOTOR_DRIVER_BB_H

#include <Arduino.h>

class ChalkbotMotorDriverBB {
    friend class ChalkbotMotorDriver;

    int16_t frontLeftSpeed = 0;
    int16_t frontRightSpeed = 0;
    int16_t rearLeftSpeed = 0;
    int16_t rearRightSpeed = 0;

public:
    void setMotorVelocity(int16_t frontLeft, int16_t frontRight, int16_t rearLeft, int16_t rearRight) {
        frontLeftSpeed = frontLeft;
        frontRightSpeed = frontRight;
        rearLeftSpeed = rearLeft;
        rearRightSpeed = rearRight;
    }
};

#endif
