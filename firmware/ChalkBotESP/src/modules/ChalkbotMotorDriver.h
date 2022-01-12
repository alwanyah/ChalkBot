#ifndef CHALKBOT_MOTOR_DRIVER_H
#define CHALKBOT_MOTOR_DRIVER_H

#include <Arduino.h>
#include "../BB.h"
#include "../util/CytronMotorDriver.h"

class ChalkbotMotorDriver
{
public:
  struct MotorPins {
    uint8_t pwmPin;
    uint8_t dirPin;
  };

  ChalkbotMotorDriver (
    const MotorPins& pins0,
    const MotorPins& pins1,
    const MotorPins& pins2,
    const MotorPins& pins3
  )
    :
    motorFrontLeft( pins0.pwmPin, pins0.dirPin),
    motorFrontRight(pins1.pwmPin, pins1.dirPin),
    motorRearLeft(  pins2.pwmPin, pins2.dirPin),
    motorRearRight( pins3.pwmPin, pins3.dirPin)
  {}

  void update() {
    motorFrontLeft.setSpeed (bb::chalkbotMotorDriver.frontLeftSpeed);
    motorFrontRight.setSpeed(bb::chalkbotMotorDriver.frontRightSpeed);
    motorRearLeft.setSpeed  (bb::chalkbotMotorDriver.rearLeftSpeed);
    motorRearRight.setSpeed (bb::chalkbotMotorDriver.rearRightSpeed);
  }

private:
  CytronMD20A motorFrontLeft;
  CytronMD20A motorFrontRight;
  CytronMD20A motorRearLeft;
  CytronMD20A motorRearRight;
};

#endif
