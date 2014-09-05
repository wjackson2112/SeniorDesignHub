// Copy and paste from the library. Too lazy to "install" it.
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class elapsedMillis
{
private:
	unsigned long ms;
public:
	elapsedMillis(void) { ms = millis(); }
	elapsedMillis(unsigned long val) { ms = millis() - val; }
	elapsedMillis(const elapsedMillis &orig) { ms = orig.ms; }
	operator unsigned long () const { return millis() - ms; }
	elapsedMillis & operator = (const elapsedMillis &rhs) { ms = rhs.ms; return *this; }
	elapsedMillis & operator = (unsigned long val) { ms = millis() - val; return *this; }
	elapsedMillis & operator -= (unsigned long val)      { ms += val ; return *this; }
	elapsedMillis & operator += (unsigned long val)      { ms -= val ; return *this; }
	elapsedMillis operator - (int val) const           { elapsedMillis r(*this); r.ms += val; return r; }
	elapsedMillis operator - (unsigned int val) const  { elapsedMillis r(*this); r.ms += val; return r; }
	elapsedMillis operator - (long val) const          { elapsedMillis r(*this); r.ms += val; return r; }
	elapsedMillis operator - (unsigned long val) const { elapsedMillis r(*this); r.ms += val; return r; }
	elapsedMillis operator + (int val) const           { elapsedMillis r(*this); r.ms -= val; return r; }
	elapsedMillis operator + (unsigned int val) const  { elapsedMillis r(*this); r.ms -= val; return r; }
	elapsedMillis operator + (long val) const          { elapsedMillis r(*this); r.ms -= val; return r; }
	elapsedMillis operator + (unsigned long val) const { elapsedMillis r(*this); r.ms -= val; return r; }
};

class elapsedMicros
{
private:
	unsigned long us;
public:
	elapsedMicros(void) { us = micros(); }
	elapsedMicros(unsigned long val) { us = micros() - val; }
	elapsedMicros(const elapsedMicros &orig) { us = orig.us; }
	operator unsigned long () const { return micros() - us; }
	elapsedMicros & operator = (const elapsedMicros &rhs) { us = rhs.us; return *this; }
	elapsedMicros & operator = (unsigned long val) { us = micros() - val; return *this; }
	elapsedMicros & operator -= (unsigned long val)      { us += val ; return *this; }
	elapsedMicros & operator += (unsigned long val)      { us -= val ; return *this; }
	elapsedMicros operator - (int val) const           { elapsedMicros r(*this); r.us += val; return r; }
	elapsedMicros operator - (unsigned int val) const  { elapsedMicros r(*this); r.us += val; return r; }
	elapsedMicros operator - (long val) const          { elapsedMicros r(*this); r.us += val; return r; }
	elapsedMicros operator - (unsigned long val) const { elapsedMicros r(*this); r.us += val; return r; }
	elapsedMicros operator + (int val) const           { elapsedMicros r(*this); r.us -= val; return r; }
	elapsedMicros operator + (unsigned int val) const  { elapsedMicros r(*this); r.us -= val; return r; }
	elapsedMicros operator + (long val) const          { elapsedMicros r(*this); r.us -= val; return r; }
	elapsedMicros operator + (unsigned long val) const { elapsedMicros r(*this); r.us -= val; return r; }
};

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

elapsedMillis timeElapsed;

#define ANALOG_UPDATE (1000) // 1Hz

void setup(){
    Serial.begin(9600);

    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
}

void loop(){
  if(timeElapsed > ANALOG_UPDATE){
    Serial.print(analogRead(PHOTORESISTOR));
    timeElapsed -= ANALOG_UPDATE;
  }

  check_input();
}
