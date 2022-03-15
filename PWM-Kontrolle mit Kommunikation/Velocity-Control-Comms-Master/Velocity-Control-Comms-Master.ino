//Code für die Kontrolle der PWM der verschiedenen Slaves

int enablePin = 8;
int counter = 0;
int increment = 1;
void setup() {
  Serial.begin(9600);
  pinMode(enablePin, OUTPUT);
  delay(10);
  digitalWrite(enablePin, HIGH);
}
/*
  Befehlsmuster:
  Anfang der Übertragung: "I"
  Trennstrich: "_"
  Nummer des anzusprenden Slave: z.B. "1"
  Trennstrich: "_"
  Zielzahl der PWM [-312; 312]: z.B. "155"
  Trennstrich: "_"
  Ende der Übertragung: "F"
*/
void loop() {
  if(abs(counter) > 312){
    increment = -increment;
  }

  //Ansprechen von Slave1
  Serial.print("I");
  Serial.print("_");
  Serial.print("1");
  Serial.print("_");
  Serial.print(counter);
  Serial.print("_");
  Serial.print("F");
  Serial.flush();

    //Ansprechen von Slave2
  Serial.print("I");
  Serial.print("_");
  Serial.print("2");
  Serial.print("_");
  Serial.print(counter);
  Serial.print("_");
  Serial.print("F");
  Serial.flush();

    //Ansprechen von Slave3
  Serial.print("I");
  Serial.print("_");
  Serial.print("3");
  Serial.print("_");
  Serial.print(counter);
  Serial.print("_");
  Serial.print("F");
  Serial.flush();

    //Ansprechen von Slave4
  Serial.print("I");
  Serial.print("_");
  Serial.print("4");
  Serial.print("_");
  Serial.print(counter);
  Serial.print("_");
  Serial.print("F");
  Serial.flush();

  counter = counter + increment;
}
