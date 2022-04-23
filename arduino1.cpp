// C++ code
//
#include <Wire.h>

#define EMPTY 0
#define NONEMPTY 1
#define DOOROPENFROMOUTSIDE 2
#define DOOROPENFROMINSIDE 3

const int slaveAddress = 10;
int state = EMPTY;
int outsideSensor = 11;
int insideSensor = 10;
int lightBulb = 8;


void setup()
{
  	pinMode(lightBulb, OUTPUT);
    Serial.begin(9600);
    Wire.begin(slaveAddress);
    Wire.onReceive(receiveEvent);
}

void receiveEvent(int something)
{
	Serial.println(Wire.read());
}

void loop()
{
  	Serial.println(digitalRead(outsideSensor));
    switch (state)
    {
    case EMPTY:
      Serial.println("Here");
        // Turn off light bulb 
        digitalWrite(lightBulb, HIGH);
        break;

    case DOOROPENFROMOUTSIDE:
        digitalWrite(lightBulb, HIGH);
        break;

    case DOOROPENFROMINSIDE:
        digitalWrite(lightBulb, HIGH);

        break;
    
    default:
        break;
    }
}

//   position = 0;
//   for (position = 1; position <= 179; position += 1) {
//     servo_9.write(position);
//     delay(20); // Wait for 20 millisecond(s)
//   }
//   for (position = 179; position >= 1; position -= 1) {
//     servo_9.write(position);
//   }
