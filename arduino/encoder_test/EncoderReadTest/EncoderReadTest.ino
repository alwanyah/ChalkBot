

int encoderPinA = 2;
int encoderPinB = 3;

#define readA bitRead(PIND,encoderPinA)//faster than digitalRead()
#define readB bitRead(PIND,encoderPinB)//faster than digitalRead()

// the setup function runs once when you press reset or power the board
void setup() 
{
  Serial.begin(11520);

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), onA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), onB, CHANGE);
}

void onA() {
  unsigned long current_time = micros();
  Serial.print('a');
  Serial.print(',');
  Serial.print(readA);
  Serial.print(',');
  Serial.print(readB);
  Serial.print(',');
  Serial.print(current_time);
  Serial.println(); 
}

void onB() {
  unsigned long current_time = micros();
  Serial.print('b');
  Serial.print(',');
  Serial.print(readA);
  Serial.print(',');
  Serial.print(readB);
  Serial.print(',');
  Serial.print(current_time);
  Serial.println(); 
}

// the loop function runs over and over again forever
void loop() {

}
