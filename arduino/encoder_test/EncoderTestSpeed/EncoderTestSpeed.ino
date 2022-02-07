
// ServoCity
// 5203 Series Yellow Jacket Planetary Gear Motor
// Ratio: 19.2:1 Ratio
// 28 pulse per revolution at the motor
// 537.6 = 28 * 19.2 pulse per revolution (at the output)
const float PPR = 537.6; 
const float radPerPulse = (2.0f * PI * 1000000.0f) / PPR;

int encoderPinA = 2;
int encoderPinB = 3;

//volatile int countA = 0;
//volatile int countB = 0;

unsigned long last_change_time = 0;
volatile float velocity = 0;

int dir = 0;  // 1 = CW
              // 0 = Stationary
              // -1 = CCW

//faster than digitalRead()
#define readA bitRead(PIND,encoderPinA)
#define readB bitRead(PIND,encoderPinB)

void setup() 
{
  Serial.begin(9600);

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), onA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), onB, CHANGE);

  last_change_time = micros();
}

void updateVelocity() 
{
  unsigned long current_time = micros();

  float dt = (current_time - last_change_time);
  velocity = radPerPulse / dt; // rad/s

  // update time
  last_change_time = current_time;
}

void onA() {
  updateVelocity();

  int a = digitalRead(encoderPinA);
  int b = digitalRead(encoderPinB);
  if (a == b) {
    velocity = -velocity;
  }
}

void onB() {
  updateVelocity();

  int a = digitalRead(encoderPinA);
  int b = digitalRead(encoderPinB);
  if (a != b) {
    velocity = -velocity;
  }
}


// the loop function runs over and over again forever
void loop() {

  // uncomment to see squarewave output
  /*
  Serial.print(digitalRead(encoderPinA));      
  Serial.print('\t');
  Serial.println(digitalRead(encoderPinB)+2);    // +2 shifts output B up so both A and B are visible 
  */
  
  // plot rising counts
  Serial.print(velocity);
  Serial.println();  
  
}
