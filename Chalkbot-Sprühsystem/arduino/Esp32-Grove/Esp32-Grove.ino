
#include "PrintDriver.h"
#include <Wire.h>
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define RefVal 3.3
  #define SERIAL SerialUSB
#else
  #define RefVal 5.0
  #define SERIAL Serial
#endif
//An OLED Display is required here
//use pin A0
#define Pin 35

PrintMD printer(25, 26, 27); //


// Motor A connections
int enA = 2;
int in1 = 4;
int in2 = 5;

// Motor B connections
int enB = 25;
int in3 = 27;
int in4 = 26;

// Take the average of 500 times
const int averageValue = 500;
 
long int sensorValue = 0;
float sensitivity = 1000.0 / 264.0; //1000mA per 264mV 
 
 
float Vref = 1269.53;   //Vref is zero drift value, you need to change this value to the value you actually measured before using it.
void setup() 
{
  SERIAL.begin(9600);
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

  getCurrent();
}

void getCurrent()
{
    sensorValue = analogRead(Pin);
    float unitValue= RefVal / 1024.0*1000 ;
    float voltage = unitValue * sensorValue; 
    float current = (voltage - Vref) * sensitivity;
    SERIAL.println(current);
}
/*void loop() 
{
  // Read the value 10 times:
  for (int i = 0; i < averageValue; i++)
  {
    sensorValue += analogRead(Pin);
 
    // wait 2 milliseconds before the next loop
    delay(2);
 
  }
 
  sensorValue = sensorValue / averageValue;
 
 
  // The on-board ADC is 10-bits 
  // Different power supply will lead to different reference sources
  // example: 2^10 = 1024 -> 5V / 1024 ~= 4.88mV
  //          unitValue= 5.0 / 1024.0*1000 ;
  float unitValue= RefVal / 1024.0*1000 ;
  float voltage = unitValue * sensorValue; 
 
  //When no load,Vref=initialValue
  //SERIAL.print("initialValue: ");
  //SERIAL.print(voltage);
  //SERIAL.println("mV"); 
 
  // Calculate the corresponding current
  float current = (voltage - Vref) * sensitivity;
 
  // Print display voltage (mV)
  // This voltage is the pin voltage corresponding to the current
  /*
  voltage = unitValue * sensorValue-Vref;
  SERIAL.print(voltage);
  SERIAL.println("mV");
  /
 
  // Print display current (mA)
  SERIAL.println(current);
  //SERIAL.println("mA");
 
  //SERIAL.print("\n");
 
  // Reset the sensorValue for the next reading
  sensorValue = 0;
  // Read it once per second
  delay(1000);
}*/
