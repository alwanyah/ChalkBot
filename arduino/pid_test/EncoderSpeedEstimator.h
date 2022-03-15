
#ifndef ENCODERSPEEDESTIMATOR_H
#define ENCODERSPEEDESTIMATOR_H

#include "Arduino.h" // must be included

//faster than digitalRead()
#define readA bitRead(PIND,encoderPinA)
#define readB bitRead(PIND,encoderPinB)


class EncoderSpeedEstimator 
{
private:
  EncoderSpeedEstimator(){};

public:
  inline static EncoderSpeedEstimator& getInstance() {
    static EncoderSpeedEstimator instance;
    return instance;
  }
  
public:
  // ServoCity
  // 5203 Series Yellow Jacket Planetary Gear Motor
  // Ratio: 19.2:1 Ratio
  // 28 pulse per revolution at the motor
  // 537.6 = 28 * 19.2 pulse per revolution (at the output)
  const float PPR = 537.6; 
  const float radPerPulse = (2.0f * PI * 1000000.0f) / PPR;

  // pins
  int encoderPinA = 2;
  int encoderPinB = 3;


  // high precision speed measurement
  volatile unsigned long last_change_time = 0;
  volatile float velocity = 0;
  volatile float dir = 0.0f;
                // 1 = CW
                // 0 = Stationary
                // -1 = CCW

  // mean speed measurement
  volatile int count;
  unsigned long last_mean_time = 0;
  float mean_velocity = 0;

  // filtered speed
  float filtered_velocity = 0;

  
  void setup() 
  {
    pinMode(encoderPinA, INPUT);
    pinMode(encoderPinB, INPUT);

    attachInterrupt(digitalPinToInterrupt(encoderPinA), callA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoderPinB), callB, CHANGE);

    last_change_time = micros();
    last_mean_time = micros();
  }

  void updateVelocity() 
  {
    count++;
    
    unsigned long current_time = micros();

    float dt = (current_time - last_change_time);
    float v = radPerPulse / dt * dir; // rad/s

    const float alpha = 0.05;
    velocity = velocity*(1.0f - alpha) + v*alpha;

    // update time
    last_change_time = current_time;
  }

  inline static void callA() { getInstance().onA(); }
  inline static void callB() { getInstance().onB(); }
  
  void onA() {
    updateVelocity();

    if(readA == readB) {
      dir = -1.0f;
    } else {
      dir = 1.0f;
    }
  }

  void onB() {
    updateVelocity();
    
    if(readA != readB) {
      dir = -1.0f;
    } else {
      dir = 1.0f;
    }
  }

  // the loop function runs over and over again forever
  float getVelocity() 
  {
    return velocity;
  }
  
  float getRPM() 
  {
    return velocity / (2.0f*PI) * 60.0;
  }
  
};

#endif
