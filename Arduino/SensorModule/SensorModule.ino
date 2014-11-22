int RED = 5;
int GREEN = 6;
int BLUE = 9;

int PHOTORESISTOR = A0;

void check_input(){
    while(Serial.available() > 0){
    int character = Serial.read();
    switch(character){
      case '0':
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, LOW);
        digitalWrite(BLUE, LOW);
        break;
      case '1':
        digitalWrite(RED, HIGH);
        digitalWrite(GREEN, LOW);
        digitalWrite(BLUE, LOW);
        break;
      case '2':
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, HIGH);
        digitalWrite(BLUE, LOW);
        break;
      case '3':
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, LOW);
        digitalWrite(BLUE, HIGH);
        break;
    }
  }
}

void setup(){
    Serial.begin(14400);

    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
}

int last = 0;

void loop(){

  delay(100);
  Serial.print(analogRead(PHOTORESISTOR));

  check_input();
}
