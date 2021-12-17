// https://github.com/CytronTechnologies/CytronMotorDriver

#ifndef CHALKBOT_MINI_MOTOR_DRIVER_H
#define CHALKBOT_MINI_MOTOR_DRIVER_H

#include "PrintDriver.h"

class ChalkbotMiniMD
{
  public:
    struct MotorPins {
      uint8_t pwmPin;
      uint8_t dirPin1;
      uint8_t dirPin2;
    };
  
    ChalkbotMiniMD (
      const MotorPins& pinsLeft,
      const MotorPins& pinsRight
    )
      : 
      motorLeft( pinsLeft.pwmPin, pinsLeft.dirPin1, pinsLeft.dirPin2),
      motorRight( pinsRight.pwmPin, pinsRight.dirPin1, pinsRight.dirPin2)
    {}

  void setMotorVelocity(int16_t v0, int16_t v1)
  {
    motorLeft.setSpeed (v0);
    motorRight.setSpeed(v1);
  }

  // methods for control
  // x - speed in the x direction 
  // rotation - rotation speed
  void setVelocityDirect(int16_t x, int16_t rotation)
  {
    // TODO: this is just a simple draft
    // NOTE: the signs depend on how the motors are built in
    motorLeft.setSpeed ( x - rotation);
    motorRight.setSpeed( x + rotation);

    // store the current command
    last_x = x;
    last_rotation = rotation;
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

  int16_t getLastX() { return last_x; }
  int16_t getLastRotation() { return last_rotation; }

  private:
    // state for the setVelocitySmooth
    float currentX = 0.0f;
    float currentRotation = 0.0f; // 

    // go to max in 1s
    float maxAcceleration = 512.0f; //255.0f / 1.0f; // x/s^2, x in [0,255]
    unsigned long lastUpdateTime = 0; // last call of setVelocitySmooth in ms

    // store last motion commands
    int16_t last_x; 
    int16_t last_rotation;
    
  public:
    PrintMD motorLeft;
    PrintMD motorRight;
};

#endif
