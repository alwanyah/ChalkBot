#include  "CytronMotorDriver.h"
//Pins der Encoder
int encoderPinA = 2;
int encoderPinB = 3;

//Pins fuer PWM und direction
int pwmPin = 5;
int dirPin = 6;

const float PPR = 537.6; 
const float radPerPulse = (2.0f * PI * 1000000.0f) / PPR;

const int testSize = 10;
volatile float values[testSize];
volatile float mean = 0;

volatile int a = 0;
volatile int b = 0;
unsigned long lastChangeTime = 0;
volatile float velocity = 0;
int dir = 0;  //1 = CW
              //0 = Stationary
              //-1= CW
CytronMD20A motor(pwmPin, dirPin);

#define readA bitRead(PIND,encoderPinA)//faster than digitalRead()
#define readB bitRead(PIND,encoderPinB)//faster than digitalRead()

void setup() {
  Serial.begin(115200);

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), onA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), onB, CHANGE);

  lastChangeTime = micros();
}

void updateVelocity()
{ 
  unsigned long current_time = micros();

  float dt = (current_time - lastChangeTime);
  velocity = radPerPulse / dt; // rad/s

  // update time
  lastChangeTime = current_time;

  //if(a >= 5){
  /*
    Serial.print(velocity); // verlocity in rad/s
    Serial.print(",");
    volatile float degs = (velocity*180.0f)/PI;
    Serial.print(degs); // verlocity in deg/s
    Serial.print(",");
    volatile float RPM = (velocity / (2.0f*PI) * 60);
    Serial.print(RPM); // RPM
    Serial.println();
  */ 
   // a = 0;
  //}else{
    //a++;
  //}
  /*if(a < testSize){
    values[a] = velocity;
    a++;
  }else{
    volatile float temp = 0;
    for(int i = 0; i < testSize; i++){
      temp = temp + values[i];
    }
    mean = temp / testSize;
    Serial.print(mean); // verlocity in rad/s
    //Serial.print(",");
    //Serial.print(mean*180.0f/PI); // verlocity in deg/s
    //Serial.print(",");
    //Serial.print(mean / (2.0f*PI) * 60.0); // RPM
    Serial.println();
    a = 0;
  }*/
  

}
void onA() {
  updateVelocity();
  
  if(readA == readB) {
  //  velocity = -velocity;
  }
/*
  volatile int a = digitalRead(encoderPinA);
  volatile int b = digitalRead(encoderPinB);
  if (a == b) {
    velocity = -velocity;
  }*//* if(a == 10){
    Serial.print(velocity);
  Serial.println();
  a = 0;
  }else{
    a++;
  }*/
  
}

void onB() {
  updateVelocity();
  
  if(readA == readB) {
  //  velocity = -velocity;
  }
  /*
  volatile int a = digitalRead(encoderPinA);
  volatile int b = digitalRead(encoderPinB);
  if (a != b) {
    velocity = -velocity;
  }*/
 /* if(b == 10){
    Serial.print(velocity);
  Serial.println();
  b = 0;
  }else{
    b++;
  }*/
}

int16_t pwm;
int mDir = 1;

void loop() 
{
  // pwm \in [-255, 255]
  motor.setSpeed(pwm);

  float degs = (velocity*180.0f)/PI;
  
  Serial.print(pwm);
  Serial.print(",");
  Serial.print(degs);
  Serial.println();

  pwm = pwm+mDir;
  if(abs(pwm) == 255)
    mDir = -mDir;
   
  delay(100);
}
