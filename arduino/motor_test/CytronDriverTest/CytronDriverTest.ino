#include  "CytronMotorDriver.h"

uint8_t pwmPin = 8; // PWM
uint8_t dirPin = 9; // direction

CytronMD20A motor(pwmPin, dirPin);

void setup()
{
  Serial.begin(115200);
  while (!Serial) {};
}

void loop()
{
  // pwm \in [-255, 255]
  int16_t pwm = 128;
  motor.setSpeed(pwm);
}
