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
    // int16_t getFrontLeftSpeed() const {
    //     return frontLeftSpeed;
    // }

    // int16_t getFrontRightSpeed() const {
    //     return frontRightSpeed;
    // }

    // int16_t getRearLeftSpeed() const {
    //     return rearLeftSpeed;
    // }

    // int16_t getRearRightSpeed() const {
    //     return rearRightSpeed;
    // }

    void setMotorVelocity(int16_t frontLeft, int16_t frontRight, int16_t rearLeft, int16_t rearRight) {
        frontLeftSpeed = frontLeft;
        frontRightSpeed = frontRight;
        rearLeftSpeed = rearLeft;
        rearRightSpeed = rearRight;
    }
};

#endif
