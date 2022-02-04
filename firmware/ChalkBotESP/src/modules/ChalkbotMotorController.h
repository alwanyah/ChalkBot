#ifndef CHALKBOT_MOTOR_CONTROLLER_H
#define CHALKBOT_MOTOR_CONTROLLER_H

#include "../BB.h"

class ChalkbotMotorController
{
public:
  void update() {
    if (!bb::chalkbotMotorController.enabled) {
      currentX = 0.0f;
      currentRotation = 0.0f;
      return;
    }

    int16_t targetForwardSpeed = bb::chalkbotMotorController.targetForwardSpeed;
    int16_t targetRotationSpeed = bb::chalkbotMotorController.targetRotationSpeed;
    bool smooth = bb::chalkbotMotorController.smooth;
    if (smooth) {
      setVelocitySmooth(targetForwardSpeed, targetRotationSpeed);
    } else {
      setVelocityDirect(targetForwardSpeed, targetRotationSpeed);
    }
  }

private:
  // methods for control
  // x - speed in the x direction
  // rotation - rotation speed
  void setVelocityDirect(int16_t x, int16_t rotation)
  {
    // TODO: this is just a simple draft
    // NOTE: the signs depend on how the motors are built in
    bb::chalkbotMotorDriver.setMotorVelocity(
      x - rotation,
      x + rotation,
      x - rotation,
      x + rotation
    );

    // store the current command
    bb::chalkbotMotorController.currentForwardSpeed = x;
    bb::chalkbotMotorController.currentRotationSpeed = rotation;
  }

  void setVelocitySmooth(float x, float rotation)
  {
    unsigned long currentTime = millis();
    float timeDelta = float(currentTime - lastUpdateTime) / 1000.0f; // in s

    float changeX = x - currentX;
    float changeRotation = rotation - currentRotation;

    float maxChange = max(abs(changeX), abs(changeRotation));
    float requestedAcceleration = maxChange/timeDelta;

    // limit the acceleration proportionally if necessary
    if( requestedAcceleration > maxAcceleration ) {
      changeX *= maxAcceleration/requestedAcceleration;
      changeRotation *= maxAcceleration/requestedAcceleration;
    }

    currentX += changeX;
    currentRotation += changeRotation;

    setVelocityDirect((int16_t)(currentX+0.5f), (int16_t)(currentRotation+0.5f));

    lastUpdateTime = currentTime;
  }

  private:
    // state for the setVelocitySmooth
    float currentX = 0.0f;
    float currentRotation = 0.0f; //

    // FIXME: move to Config.h
    // go to max in 1s
    static constexpr float maxAcceleration = 512.0f; //255.0f / 1.0f; // x/s^2, x in [0,255]
    unsigned long lastUpdateTime = 0; // last call of setVelocitySmooth in ms
};

#endif
