#include  "CytronMotorDriver.h"
//Pins der Encoder
int encoderPinA = 2;
int encoderPinB = 3;

//Pins fuer PWM und direction
int pwmPin = 5;
int dirPin = 6;

volatile int a = 0;
volatile int b = 0;
CytronMD20A motor(pwmPin, dirPin);

#define readA bitRead(PIND,encoderPinA)//faster than digitalRead()
#define readB bitRead(PIND,encoderPinB)//faster than digitalRead()

void setup() {
  Serial.begin(115200);

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), onA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), onB, CHANGE);
}

void onA() {
  unsigned long current_time = micros();
  if(a == 10){
    Serial.print('a');
    Serial.print(',');
    Serial.print(readA);
    Serial.print(',');
    Serial.print(readB);
    Serial.print(',');
    Serial.print(current_time);
    Serial.println();
    a = 0;
  }else{
    a++;
  }
}

void onB() {
  unsigned long current_time = micros();
  if(b == 10){
    Serial.print('b');
    Serial.print(',');
    Serial.print(readA);
    Serial.print(',');
    Serial.print(readB);
    Serial.print(',');
    Serial.print(current_time);
    Serial.println(); 
    b = 0;
  }else{
    b++;
  }
}
void loop() {
    for(int i = 0; i < 256; i++){
      noInterrupts();
  // pwm \in [-255, 255]
  int16_t pwm = i;
  motor.setSpeed(pwm);
  Serial.print(pwm);
  Serial.println();
  Serial.print("-----------------------------------");
  Serial.println();
  interrupts();
  delay(100);
  }
  Serial.end();
}
