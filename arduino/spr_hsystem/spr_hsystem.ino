#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

// Motor A connections
int enA = 9;
int in1 = 8;
int in2 = 7;

// Motor B connections
int enB = 3;
int in3 = 5;
int in4 = 4;


  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

void setup() 
{
  // Set all the motor control pins to outputs
  //pinMode(enA, OUTPUT);

  TCCR2B = TCCR2B & B11111000 | B00000001;
  
  pinMode(enB, OUTPUT);
  //pinMode(in1, OUTPUT);
  //pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT); 
  
  // Turn off motors - Initial state
  //digitalWrite(in1, LOW);
  //digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
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
      state = 0;
    }

    //
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enB, 255);
  } 
  else if(state == 1) 
  {
    if(stateTime > 1000) {
      state = 0;
    }

    //
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enB, 0);
  }

  if(lastState != state) {
    t0 = millis();
  }
  lastState = state;

  getVoltage();
  
  //analogWrite(enA, 0);
  //analogWrite(enB, 0);
  //digitalWrite(in1, HIGH);
  //digitalWrite(in2, LOW);

  /*
  //Serial.println("in3=L, in4=H, enB=255");
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 230);
  for (int i = 130; i <=255; i++) {
    //Serial.print(i);Serial.println(" ");
    analogWrite(enB, i);
    delay(100);
  }
  
  //delay(300);
  for (int i = 255; i >= 130; --i) {
    //Serial.print(i);Serial.println(" ");
    analogWrite(enB, i);
    delay(100);
  }
  //Serial.print(ina219.getBusVoltage_V()); Serial.println(" ");
  //getVoltage();
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enB, 0);
  delay(100);
  */
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

// This function lets you control spinning direction of motors
void directionControl() {
  // Set motors to maximum speed
  // For PWM maximum possible values are 0 to 255
  analogWrite(enA, 255);
  analogWrite(enB, 255);

  // Turn on motor A & B
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(2000);
  
  // Now change motor directions
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(2000);
  
  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// This function lets you control speed of the motors
void speedControl() {
  // Turn on motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  // Accelerate from zero to maximum speed
  for (int i = 0; i < 256; i++) {
    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(20);
  }
  
  // Decelerate from maximum speed to zero
  for (int i = 255; i >= 0; --i) {
    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(20);
  }
  
  // Now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
