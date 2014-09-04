#ifdef __MK20DX256__
#include <elapsedMillis.h>

// Serial1: 0 = RX1, 1 = TX1
int RED = 13; // Teensy LED
int GREEN = 2;
int BLUE = 3;

int PHOTORESISTOR = A0;
#else // Stock Arduino
int RED = 5;
int GREEN = 6;
int BLUE = 9;

int PHOTORESISTOR = A0;
#endif // __MK20DX128__

void check_input(){
#ifdef __MK20DX256__
    while(Serial1.available() > 0){
    int character = Serial1.read();
#else // Stock Arduino
    while(Serial.available() > 0){
    int character = Serial.read();
#endif // __MK20DX128__
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

#ifdef __MK20DX256__
elapsedMillis timeElapsed;

#define ANALOG_UPDATE (1000) // 1Hz
#endif // __MK20DX128__

void setup(){
#ifdef __MK20DX256__
    Serial1.begin(9600);
#else // Stock Arduino
    Serial.begin(9600);
#endif // __MK20DX128__

    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
}

int last = 0;

void loop(){
#ifdef __MK20DX256__
  if(timeElapsed > ANALOG_UPDATE){
    last += 10;
    if(last > 1023) last = 0;
    //Serial1.print(analogRead(PHOTORESISTOR));
    Serial1.print(last);
    timeElapsed -= 1000;
  }
#else // Stock Arduino
  Serial.print(analogRead(PHOTORESISTOR));
#endif // __MK20DX128__

  check_input();
}
