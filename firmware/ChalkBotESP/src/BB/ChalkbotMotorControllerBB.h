#ifndef CHALKBOT_MOTOR_CONTROLLER_BB_H
#define CHALKBOT_MOTOR_CONTROLLER_BB_H

#include <Arduino.h>

class ChalkbotMotorControllerBB {
    friend class ChalkbotMotorController;

    // FIXME: replace with float in [-1.0, 1.0]
    int16_t targetForwardSpeed = 0; // [-255, 255]
    int16_t targetRotationSpeed = 0; // [-255, 255]
    int16_t currentForwardSpeed = 0; // [-255, 255]
    int16_t currentRotationSpeed = 0; // [-255, 255]
    bool smooth = false;
    bool enabled = false;

public:
    // methods for control
    // x - speed in the x direction
    // rotation - rotation speed
    void setVelocityDirect(int16_t forwardSpeed, int16_t rotationSpeed) {
        targetForwardSpeed = forwardSpeed;
        targetRotationSpeed = rotationSpeed;
        smooth = false;
        enabled = true;
    }

    void setVelocitySmooth(int16_t forwardSpeed, int16_t rotationSpeed) {
        targetForwardSpeed = forwardSpeed;
        targetRotationSpeed = rotationSpeed;
        smooth = true;
        enabled = true;
    }

    void disable() {
        enabled = false;
    }

    int16_t getCurrentForwardSpeed() const {
        return currentForwardSpeed;
    }

    int16_t getCurrentRotationSpeed() const {
        return currentRotationSpeed;
    }
};

#endif
