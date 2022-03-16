
// http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/
// https://github.com/imax9000/Arduino-PID-Library

#include <PID_v2.h>
#include "CytronMotorDriver.h"
#include "EncoderSpeedEstimator.h"

//Pins fuer PWM und direction
int pwmPin = 5;
int dirPin = 6;
CytronMD20A motor(pwmPin, dirPin);

double Kp = 0.20, Ki = 0.20, Kd = 0;
PID_v2 motorPID(Kp, Ki, Kd, PID::Direct);

EncoderSpeedEstimator& encoder = EncoderSpeedEstimator::getInstance();

void setup()
{
  // PWM
  motorPID.SetOutputLimits(0, 255);
  //sample time in ms for stability
  motorPID.SetSampleTime(100);
  // input_, currentOutput, setpoint_
  motorPID.Start(0,0,100);

  encoder.setup();

  // debug
  Serial.begin(115200);
  while(!Serial);
}

void loop()
{
  // Read the position in an atomic block to avoid a potential
  // misread if the interrupt coincides with this code running
  // see: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
  int measuredVelocity = 0; 
  measuredVelocity = encoder.getRPM();
  
  const double output = motorPID.Run(measuredVelocity);
  motor.setSpeed((int)output);
}
