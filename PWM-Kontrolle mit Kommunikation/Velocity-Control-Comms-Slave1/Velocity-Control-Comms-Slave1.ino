#include  "CytronMotorDriver.h"
#include <util/atomic.h>

//Kontrolle der PWM durch Kommunikation mit Master
//Slavecode 1
//Pins
#define ENCA 2
#define ENCB 3
#define PWM 5
#define DIR 6

//Anpassen um Slavenummer zu ändern
const int slaveNum = 1;


int enablePin = 8;
//Counts per output-shaft-rotation
const float PPR = 134.4; // 134.4 oder 537,6

//Globals
long prevT = 0;
int posPrev = 0;

//variables used interrupt
volatile int pos_i = 0;
volatile float velocity_i = 0;
volatile long prevT_i = 0;

float v1Filt = 0;
float v1Prev = 0;
float v2Filt = 0;
float v2Prev = 0;

float eintegral = 0;


int targetVT = 0; //Wird vom Master gesetzt

//Setzen der Motorpins;
CytronMD20A motor(PWM, DIR);

void setup() {
  Serial.begin(9600);
  
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  pinMode(PWM, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(enablePin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);
  delay(10);
  digitalWrite(enablePin, LOW);
}

void loop() {

  int pos = 0;
  float velocity2 = 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    pos = pos_i;
    velocity2 = velocity_i;
  }

  //Compute Velocity using method 1
  long currT = micros();
  float deltaT = ((float)(currT-prevT))/1.0e6;
  float velocity1 = (pos - posPrev)/deltaT;
  posPrev = pos;
  prevT = currT;

  //Convert counts/s to RPM
  float v1 = velocity1/PPR*60;
  float v2 = velocity2/PPR*60;

/*
  VT = 10 - 107 => 0.584*v2Filt + 0.0728*v2 + 0.0728*v2Prev;
  VT = 108 - 150 => 0.7*v2Filt + 0.1*v2 + 0.1*v2Prev;
  VT = 151 - 199 => 0.75*v2Filt + 0.1*v2 + 0.1*v2Prev;
  VT = 200 - 280 => 0.8*v2Filt + 0.1*v2 + 0.1*v2Prev;
  VT = 281 - 312 => 0.81*v2Filt + 0.1*v2 + 0.1*v2Prev;
  */

  
  if(Serial.available()){
    if(Serial.read() == 'I'){
      if(Serial.read() == '_'){
        int Slave = Serial.parseInt();
        if(slaveNum == Slave){
          if(Serial.read() == '_'){
            int temp = Serial.parseInt();
            if(Serial.read() == '_'){
              if(Serial.read() == 'F'){
                targetVT = temp;
              }
            }
          }
        }
      }
    }
  }

  
  //Set a target in RPM
  float vt = targetVT;// *(sin(currT/1e6)>0);
  if(abs(vt) >= 10 && abs(vt) <= 107){
    v2Filt = 0.584*v2Filt + 0.0728*v2 + 0.0728*v2Prev;
    v2Prev = v2;
  }else if(abs(vt) <= 150){
    v2Filt = 0.7*v2Filt + 0.1*v2 + 0.1*v2Prev;
    v2Prev = v2;
  }else if(abs(vt) <= 199){
    v2Filt = 0.75*v2Filt + 0.1*v2 + 0.1*v2Prev;
    v2Prev = v2;
  }else if(abs(vt) <= 280){
    v2Filt = 0.8*v2Filt + 0.1*v2 + 0.1*v2Prev;
    v2Prev = v2;
  }else{
    v2Filt = 0.81*v2Filt + 0.1*v2 + 0.1*v2Prev;
    v2Prev = v2;
  }


  /*
  //low-pass filter (25Hz cutoff)
  v1Filt = 0.584*v1Filt + 0.0728*v1 + 0.0728*v1Prev;
  v1Prev = v1;
  v2Filt = 0.8*v2Filt + 0.1*v2 + 0.1*v2Prev;
  v2Prev = v2;
*/


  //Compute the control signal u
  float kp = 5;
  float ki = 10;
  float e = vt-v2Filt;
  eintegral = eintegral + e*deltaT;

  float u = kp*e + ki*eintegral;

//TODO Angucken wie die Motorkontrolle gesetzt und berechnet werden muss
//______________________________________________________________________
  //Set the motor speed and direction
  int dir = 1;
  if(u < 0){
    dir = -1;
  }

  int pwm = (int) fabs(u);
  if(abs(targetVT) <= 10){
    pwm = 0; 
  }
  pwm = pwm * dir;
  if(pwm > 255){
    pwm = 255;
  }
  motor.setSpeed(pwm);

  
  Serial.print(vt);
  Serial.print(" ");
  Serial.print(v2Filt);
  Serial.println();
  delay(1);
}

void readEncoder(){
  // Read encoder B when ENCA rises
  int b = digitalRead(ENCB);
  int increment = 0;
  if(b>0){
    // If B is high, increment forward
    increment = 1;
  }
  else{
    // Otherwise, increment backward
    increment = -1;
  }
  pos_i = pos_i + increment;

  // Compute velocity with method 2
  long currT = micros();
  float deltaT = ((float) (currT - prevT_i))/1.0e6;
  velocity_i = increment/deltaT;
  prevT_i = currT;
}
