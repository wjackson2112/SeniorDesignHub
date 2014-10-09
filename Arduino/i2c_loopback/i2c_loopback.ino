#include <Wire.h>

int RED = 5;
int GREEN = 6;
int BLUE = 9;

int PHOTORESISTOR = A0;

void setup()
{
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void loop()
{
  delay(10); 
}

//Write command received
void receiveEvent(int howMany){
  while(0 < Wire.available())
  {
    char character = Wire.read();
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
    Serial.print(character);
  }
  Serial.println();
}

//Read command received
void requestEvent(){
  int value = analogRead(PHOTORESISTOR);
  char output[4];
  
  String str = String(value);
  str.toCharArray(output, 4);
  
  Serial.println(value);
  Wire.write((uint8_t * )output, 4);
}
