// https://github.com/CytronTechnologies/CytronMotorDriver

#ifndef CHALKBOT_MOTOR_DRIVER_H
#define CHALKBOT_MOTOR_DRIVER_H

#include "CytronMotorDriver.h"

class ChalkbotMD
{
  public:
    struct MotorPins {
      uint8_t pwmPin;
      uint8_t dirPin;
    };
  
    ChalkbotMD (
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

  void setMotorVelocity(int16_t v0, int16_t v1, int16_t v2, int16_t v3)
  {
    motorFrontLeft.setSpeed (v0);
    motorFrontRight.setSpeed(v1);
    motorRearLeft.setSpeed  (v2);
    motorRearRight.setSpeed (v3);
  }

  // methods for control
  // x - speed in the x direction 
  // rotation - rotation speed
  void setVelocityDirect(int16_t x, int16_t rotation)
  {
    // TODO: this is just a simple draft
    // NOTE: the signs depend on how the motors are built in
    motorFrontLeft.setSpeed ( x - rotation);
    motorFrontRight.setSpeed( x + rotation);
    motorRearLeft.setSpeed  ( x - rotation);
    motorRearRight.setSpeed ( x + rotation);

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
    CytronMD20A motorFrontLeft;
    CytronMD20A motorFrontRight;
    CytronMD20A motorRearLeft;
    CytronMD20A motorRearRight;
};

#endif
