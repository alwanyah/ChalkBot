


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

void onA() {
  updateVelocity();

  /*
  int a = digitalRead(encoderPinA);
  int b = digitalRead(encoderPinB);
  if (a == b) {
    velocity = -velocity;
  }
  */

  if(readA == readB) {
    dir = -1.0f;
  } else {
    dir = 1.0f;
  }
}

void onB() {
  updateVelocity();

  /*
  int a = digitalRead(encoderPinA);
  int b = digitalRead(encoderPinB);
  if (a != b) {
    velocity = -velocity;
  }
  */

  if(readA != readB) {
    dir = -1.0f;
  } else {
    dir = 1.0f;
  }
}


// the loop function runs over and over again forever
void loop() 
{
  // estimate mean velocity
  unsigned long current_time = micros();
  if(current_time > last_mean_time + 1000000/10) { // 1s
    float dt = (current_time - last_mean_time);
    mean_velocity = ((float)count) * radPerPulse * dir / dt;

    if(count == 0) {
      velocity = 0;
    }
    
    count = 0;
    last_mean_time = current_time;
  }

  filtered_velocity = 0.9f*mean_velocity + 0.1*velocity;

  // uncomment to see squarewave output
  /*
  Serial.print(digitalRead(encoderPinA));      
  Serial.print('\t');
  Serial.println(digitalRead(encoderPinB)+2);    // +2 shifts output B up so both A and B are visible 
  */
  
  // plot rising counts
  
  Serial.print(velocity); // verlocity in rad/s
  Serial.print(",");
  //Serial.print(velocity*180.0f/PI); // verlocity in deg/s
  //Serial.print(",");
  Serial.print(velocity / (2.0f*PI) * 60.0); // RPM
  Serial.print(",");
  
  Serial.print(filtered_velocity); // verlocity in rad/s
  Serial.print(",");
  //Serial.print(filtered_velocity*180.0f/PI); // verlocity in deg/s
  //Serial.print(",");
  Serial.print(filtered_velocity / (2.0f*PI) * 60.0); // RPM
  
  Serial.println();  
  
}
