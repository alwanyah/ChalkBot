#ifndef PRINTER_H
#define PRINTER_H

#include <Arduino.h>

class PrinterClass
{
  public:
    PrinterClass(uint8_t pinPwm, uint8_t pinDir1, uint8_t pinDir2) 
      : 
      //pinPwm(pinPwm),
      pinDir1(pinDir1),
      pinDir2(pinDir2)
    {
        pinMode(pinPwm, OUTPUT);
        pinMode(pinDir1, OUTPUT);
        pinMode(pinDir2, OUTPUT);
        
        digitalWrite(pinPwm, LOW);
        digitalWrite(pinDir1, LOW);
        digitalWrite(pinDir2, LOW);

        // Setting PWM properties
        const int freq = 20000;
        const int pwmChannel = 0;
        const int resolution = 8;

        // configure LED PWM functionalitites
        ledcSetup(pwmChannel, freq, resolution);
        
        // attach the channel to the GPIO to be controlled
        ledcAttachPin(pinPwm, pwmChannel);
        pinPwm = pwmChannel;
    }
    
    void setSpeed(int16_t speed) 
    {
      // Make sure the speed is within the limit.
      if (speed > 255) {
        speed = 255;
      } else if (speed < -255) {
        speed = -255;
      }

      if (speed >= 0) {
        // direction: // out4 ~ '+', out3 ~ '-'
        digitalWrite(pinDir1, LOW);
        digitalWrite(pinDir2, HIGH);
        //analogWrite(pinPwm, speed);
        ledcWrite(pinPwm, speed);
      } else {
        // direction: // out4 ~ '-', out3 ~ '+'
        digitalWrite(pinDir1, HIGH);
        digitalWrite(pinDir2, LOW);
        //analogWrite(pinPwm, -speed);
        ledcWrite(pinPwm, -speed);
      }
    }
    
  private:
    uint8_t pinPwm;

    // NOTE: directional pins
    uint8_t pinDir1;
    uint8_t pinDir2;
};

extern PrinterClass Printer;

#endif
