#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

// Motor A connections
int enA = 14;
int in1 = 27;
int in2 = 26;

// Motor B connections
int enB = 32;
int in3 = 33;
int in4 = 25;

// L298n -> Esp32
// enA -> D14
// in1 -> D27
// in2 -> D26
// enB -> D6
// in3 -> D3
// in4 -> D4

int x =0;


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
  //Serial.println("start");
}

long t0 = millis();
int state = 0;
int lastState = 0;
int loopamount = 0;

void loop() 
{
 // long stateTime = millis() - t0;
  /*
  if(state == 0)
  {
    if(loopamount > 3) {
      state = 1;
      loopamount=0;
    }
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    digitalWrite(enB, HIGH);
  } 
  else if(state == 1) 
  {
    if(loopamount > 7) {
      state = 0;
      loopamount=0;
    }
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    digitalWrite(enB, 0);
  }
  loopamount=loopamount+1;*/
  digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    digitalWrite(enB, HIGH);
  //Serial.print(state); 
  //Serial.print(" ");
  getVoltage();
  delay(10);

}

void getVoltage()
{
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print(busvoltage); 
  Serial.print(" ");
  Serial.print(shuntvoltage); Serial.print(" ");
  Serial.print(loadvoltage); Serial.print(" ");
  Serial.print(current_mA); Serial.print(" ");
  Serial.println(power_mW);
  
}

// This function lets you control spinning direction of motors
void directionControl() {
  // Set motors to maximum speed
  // For PWM maximum possible values are 0 to 255
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);

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
    digitalWrite(enA, i);
    digitalWrite(enB, i);
    delay(20);
  }
  
  // Decelerate from maximum speed to zero
  for (int i = 255; i >= 0; --i) {
    digitalWrite(enA, i);
    digitalWrite(enB, i);
    delay(20);
  }
  
  // Now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
