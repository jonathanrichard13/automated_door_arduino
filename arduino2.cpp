// C++ code
//
#include <Wire.h>

#define CLOSED 0
#define OPENED_FROM_INSIDE 1
#define OPENED_FROM_OUTSIDE 2
#define MOVE_DETECTED_TO_EXIT 3
#define MOVE_DETECTED_TO_ENTER 4
#define MOVE_DETECTED_EXITING 5
#define MOVE_DETECTED_ENTERING 6

const int masterAddress = 1;
const int slaveAddress = 10;
int state = CLOSED;
int outsideSensor = 11;
int insideSensor = 10;
int lightBulb = 8;
int button = 2;
int peopleCount;
int buttonPushed = LOW;
unsigned long start = 0;


void setup()
{
    Serial.begin(9600);
    Wire.begin(slaveAddress);
    Wire.onReceive(receiveEvent);
    peopleCount = 0;
    pinMode(button, INPUT);
    pinMode(lightBulb, OUTPUT);
}

void receiveEvent(int something)
{
    Wire.read();
	state = OPENED_FROM_OUTSIDE;
    start = millis();
}

void loop()
{
    if (peopleCount > 0){
        digitalWrite(lightBulb, HIGH);
    } else {
        digitalWrite(lightBulb, LOW);
    }
    Serial.println(millis() - start);
    Serial.println(state);

    buttonPushed = digitalRead(button);

    switch (state)
    {
    case CLOSED:
        if (buttonPushed == HIGH){
            openDoor();
            state = OPENED_FROM_INSIDE;
            start = millis();
        }
        break;

    case OPENED_FROM_INSIDE:
        if (millis() - start < 10000){
            if (digitalRead(insideSensor) == HIGH){
                state = MOVE_DETECTED_TO_EXIT;  
            } else if (digitalRead(outsideSensor) == HIGH){
                state = MOVE_DETECTED_ENTERING;
            }
        } else {
            state = CLOSED;
            closeDoor();
        }
        break;

    case OPENED_FROM_OUTSIDE:
        if (millis() - start < 10000){
            if (digitalRead(outsideSensor) == HIGH){
                state = MOVE_DETECTED_TO_ENTER;  
            } else if (digitalRead(insideSensor) == HIGH){
                state = MOVE_DETECTED_TO_EXIT;
            }
        } else {
            state = CLOSED;
            closeDoor();
        }
        break;
    
    case MOVE_DETECTED_TO_EXIT:
        if (millis() - start < 10000){
            if (digitalRead(outsideSensor) == HIGH){
                state = MOVE_DETECTED_EXITING;
            } else if (digitalRead(insideSensor) == LOW){
                state = OPENED_FROM_INSIDE;
            }
        } else {
            state = CLOSED;
            closeDoor();
        }
        break;

    case MOVE_DETECTED_TO_ENTER:
        if (millis() - start < 10000){
            if (digitalRead(insideSensor) == HIGH){
                state = MOVE_DETECTED_ENTERING;
            } else if (digitalRead(outsideSensor) == LOW){
                state = OPENED_FROM_OUTSIDE;
            }
        } else {
            state = CLOSED;
            closeDoor();
        }
        break;

    case MOVE_DETECTED_EXITING:
        if (millis() - start < 10000){
            if (digitalRead(insideSensor) == LOW){
                peopleCount--;
                state = CLOSED;
                closeDoor();
            } else if (digitalRead(outsideSensor) == LOW){
                state = MOVE_DETECTED_TO_EXIT;
            }
        } else {
            state = CLOSED;
            closeDoor();
        }
        break;

    case MOVE_DETECTED_ENTERING:
        if (millis() - start < 10000){
            if (digitalRead(outsideSensor) == LOW){
                peopleCount++;
                state = CLOSED;
                closeDoor();
            } else if (digitalRead(insideSensor) == LOW){
                state = MOVE_DETECTED_TO_ENTER;
            }
        } else {
            state = CLOSED;
            closeDoor();
        }
        break;

    default:
        break;
    }
}


void openDoor(){
    Wire.beginTransmission(masterAddress);
    Wire.write(1);
    Wire.endTransmission();
}

void closeDoor(){
    Wire.beginTransmission(masterAddress);
    Wire.write(0);
    Wire.endTransmission(); 
}


//   position = 0;
//   for (position = 1; position <= 179; position += 1) {
//     servo_9.write(position);
//     delay(20); // Wait for 20 millisecond(s)
//   }
//   for (position = 179; position >= 1; position -= 1) {
//     servo_9.write(position);
//   }

