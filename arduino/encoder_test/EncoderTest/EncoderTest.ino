


int encoderPinA = 2;
int encoderPinB = 3;

volatile int countA = 0;
volatile int countB = 0;

int dir = 0;  // 1 = CW
              // 0 = Stationary
              // -1 = CCW

#define readA bitRead(PIND,encoderPinA)//faster than digitalRead()
#define readB bitRead(PIND,encoderPinB)//faster than digitalRead()

// the setup function runs once when you press reset or power the board
void setup() 
{
  Serial.begin(9600);

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), isrB, RISING);
}

void isrA() {
  if(digitalRead(encoderPinB) ==  HIGH) {
    dir = 1;
  } else {
    dir = -1;
  }

  countA += dir;
}

void isrB() {
  countB += dir;
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
  Serial.print(countA);
  Serial.print(',');
  Serial.print(countB);
  Serial.println();  
  
}
