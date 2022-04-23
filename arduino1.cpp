#include<LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>


#define UNACTIVE 0
#define ACTIVE 1
#define UNLOCKED 2
#define TIMEOUT 3

// SERVO
Servo servo;

// LCD
LiquidCrystal lcd(12, 11, A0, A1, A2, A3);

const byte numRows= 4;
const byte numCols= 4;

// KEYPAD
char keymap[numRows][numCols]= 
{
    {'1', '2', '3', 'A'}, 
    {'4', '5', '6', 'B'}, 
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {9,8,7,6}; //Rows 0 to 3
byte colPins[numCols]= {5,4,3,2}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

int state = UNACTIVE;
unsigned long start;
unsigned long wrongPasswordStart = 0;
String inputCode = ""; 
char keypressed = '#';
String passcode = "1234";
int recv;

const int masterAddress = 1;
const int slaveAddress = 10;

void setup()
{
    Serial.begin(9600);
    lcd.begin(16, 2); 
    servo.attach(10, 500, 2500);
    servo.write(0);
    Wire.begin(1);
    Wire.onReceive(receiveEvent);
}

void receiveEvent(int something)
{
	Serial.println("RECEIVED");
    recv = Wire.read();
    if (recv == 1){
        servo.write(90);
    } else {
        servo.write(0);
    }
}

void loop()
{
    keypressed = myKeypad.getKey();
    switch (state)
    {
        case UNACTIVE:
            if (keypressed == 'A'){
                start = millis();
                state = ACTIVE;
            } else {
            	lcd.clear();
            }
            break;
        case ACTIVE:
      		if (millis() - wrongPasswordStart < 500){
      			// Do nothing
            }
            else if (inputCode.length() == 4){
                checkPassword();
            } else {
                promptStart(15);
            }
            break;
        case UNLOCKED:
                if (millis() - start < 2000){
                    lcd.setCursor(0, 0);
                    lcd.print("UNLOCKED        ");
                    clearLCD(1);
                } else {
                    inputCode = "";
                    state = UNACTIVE;
                }
            break;
        case TIMEOUT:
      		timeout();
            break;
        default:
            break;
    }

}



void promptStart(int countdown){
    lcd.setCursor(0, 0);
    lcd.print("Password: ");
    if (millis() - start < countdown*1000){
        lcd.setCursor(0, 1);
        lcd.print("Time left: " + (String) (countdown - (millis() - start)/1000));
        keypressed = myKeypad.getKey();
        if (isDigit(keypressed)){
            inputCode += keypressed;
        }

        lcd.setCursor(10, 0);
        lcd.print(inputCode);
    } else {
        state = TIMEOUT;
      	start = millis();
    }
}

void checkPassword(){
    if (inputCode == passcode){
        start = millis();
        state = UNLOCKED;

        Wire.beginTransmission(slaveAddress);
        Wire.write(true);
        Wire.endTransmission();
        // Open door
        servo.write(90);
    } else {
        inputCode = "";
        lcd.setCursor(0, 0);
        lcd.print("INCORRECT       ");
        lcd.setCursor(0, 1);
        lcd.print("PASSWORD        ");
        wrongPasswordStart = millis();
    }
}

void timeout(){
    if (millis() - start < 10000){
        lcd.setCursor(0, 0);
        lcd.print("TIMEOUT         ");
        lcd.setCursor(0, 1);
        lcd.print("Waiting time: " + ((String) (10 - (millis() - start)/1000)) + " ");
    } else {
        state = UNACTIVE; 
    }
}

void clearLCD(int row){
    lcd.setCursor(0, row);
    lcd.print("                "); 
}
