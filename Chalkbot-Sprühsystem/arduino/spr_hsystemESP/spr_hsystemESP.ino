
#include "PrintDriver.h"
#include <Wire.h>
#include <Adafruit_INA219.h>

PrintMD printer(25, 26, 27); // 

Adafruit_INA219 ina219;

// Motor A connections
int enA = 2;
int in1 = 4;
int in2 = 5;
/*Arduino Nano
int enA = 9;
int in1 = 8;
int in2 = 7;*/

// Motor B connections
int enB = 25;
int in3 = 27;
int in4 = 26;
/*Arduino Nano
int enB = 3;
int in3 = 5;
int in4 = 4;*/


  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

void setup() 
{
  // Set all the motor control pins to outputs
  //pinMode(enA, OUTPUT);

  //TCCR2B = TCCR2B & B11111000 | B00000001;
  
  //pinMode(enB, OUTPUT);
  //pinMode(in1, OUTPUT);
  //pinMode(in2, OUTPUT);
  //pinMode(in3, OUTPUT);
  //pinMode(in4, OUTPUT); 
  
  // Turn off motors - Initial state
  //digitalWrite(in1, LOW);
  //digitalWrite(in2, LOW);
  //digitalWrite(in3, LOW);
  //digitalWrite(in4, LOW);
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  Serial.begin(9600);
}

long t0 = millis();
int state = 0;
int lastState = 0;

void loop() 
{
  long stateTime = millis() - t0;
  
  if(state == 0)
  {
    if(stateTime > 1000) {
      state = 1;
    }

    printer.setSpeed(255);
    //
    //digitalWrite(in3, LOW);
    //digitalWrite(in4, HIGH);
    //analogWrite(enB, 255);
  } 
  else if(state == 1) 
  {
    if(stateTime > 1000) {
      state = 0;
    }

    printer.setSpeed(0);
    //
    //digitalWrite(in3, LOW);
    //digitalWrite(in4, HIGH);
    //analogWrite(enB, 0);
  }

  if(lastState != state) {
    t0 = millis();
  }
  lastState = state;

  getVoltage();
}
  
  


void getVoltage()
{
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print(busvoltage); Serial.println(" ");
  //Serial.print(shuntvoltage); Serial.println(" ");
  //Serial.print(loadvoltage); Serial.println(" ");
  //Serial.print(current_mA); Serial.println(" ");
  //Serial.print(power_mW); Serial.println(" ");
  //Serial.println("");
  
}
