#include <Arduino.h>
#include <TimeLib.h>
#include <DS1307RTC.h> // a basic DS1307 library that returns time as a time_t
#include <Wire.h>
#include <TimeAlarms.h>
#include <ezButton.h>

#include "config.h"

//#include <SNTPtime.h>

const int greenButtonPin = 12;
const int redButtonPin = 11;
int greenButtonState;                    // the current reading from the input pin
int lastGreenButtonState = HIGH;         // the previous reading from the input pin
int redButtonState;                      // the current reading from the input pin
int lastRedButtonState = HIGH;           // the previous reading from the input pin
unsigned long lastDebounceTimeGreen = 0; // the last time the output pin was toggled
unsigned long lastDebounceTimeRed = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;        // the debounce time; increase if the output flickers
ezButton greenButton(greenButtonPin);
ezButton redButton(redButtonPin);

//Watering Cycle Timing and Pump Control
const int pumpControl = 10;
unsigned long currentMillis;
unsigned long endMillis; //some global variables available anywhere in the program
unsigned long primingMillis;
unsigned long wateringMillis;
const unsigned long primeLinePeriod = 5000;     //prime water line at 6v for 10sec
const unsigned long wateringCyclePeriod = 5000; //run watering cycle at 9v for 3min
int lastCycle;
const int poweredOn = 1;
const int cycleStart = 2;
const int primingCycle = 3;
const int wateringCycle = 4;
const int endCycle = 5;

//LED's
const int blueLed = 4;
const int greenLed = 5;
int allLed[] = {blueLed, greenLed};
int ledCount = 2;

//blink
// const unsigned long blinkDelay = 500;
// unsigned long blinkMillis = 0;
// unsigned long newBlinkMillis = 0;
// int blueLedState;

//Functions

// void blinkBlueLed()
// {
//   // check to see if it's time to blink the LED; that is, if the difference
//   // between the current time and last time you blinked the LED is bigger than
//   // the interval at which you want to blink the LED.
//   unsigned long newBlinkMillis = millis();

//   if (newBlinkMillis - blinkMillis >= blinkDelay)
//   {
//     // save the last time you blinked the LED
//     newBlinkMillis = millis();

//     // if the LED is off turn it on and vice-versa:
//     if (blueLedState == LOW)
//     {
//       blueLedState = HIGH;
//     }
//     else
//     {
//       blueLedState = LOW;
//     }

//     // set the LED with the ledState of the variable:
//     digitalWrite(blueLed, blueLedState);
//   }
// }

void printDigits(int digits);
void digitalClockDisplay();

void runCycleStart();
void runPrimeLine();
void runWateringCycle();
void endWateringCycle();




// the setup routine runs once when you press reset:
void setup()
{
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  setSyncProvider(RTC.get); // the function to get the time from the RTC

  greenButton.setDebounceTime(50);
  redButton.setDebounceTime(50);

  //LED'S
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  //Water Pump
  pinMode(pumpControl, OUTPUT);
  digitalWrite(pumpControl, LOW);
  digitalWrite(blueLed, LOW);
  endMillis = 0;
  primingMillis = 0;
  wateringMillis = 0;
  lastCycle = poweredOn;
  Serial.println("Powered On");

  //time_t t = now();
  Alarm.timerOnce(5, runCycleStart); //call function a number of seconds after startup
}

void loop()
{
  currentMillis = millis();
  Alarm.delay(0);

  greenButton.loop();
  redButton.loop();

  if (greenButton.isPressed())
  {
    runCycleStart();
  }

  if (redButton.isPressed() && lastCycle != endCycle)
  {
    endWateringCycle();
  }

  if (lastCycle == cycleStart)
  {
    runPrimeLine();
  }

  if (lastCycle == primingCycle && currentMillis - wateringMillis <= wateringCyclePeriod && currentMillis > wateringMillis)
  {
    runWateringCycle();
  }

  if (lastCycle == wateringCycle && currentMillis >= endMillis)
  {
    endWateringCycle();
  }
}

void printDigits(int digits)
{
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print("-");
  Serial.print(month());
  Serial.print("-");
  Serial.print(year());
  Serial.println();
}

void runPrimeLine()
{
  //if (currentMillis - primingMillis <= primeLinePeriod){
  primingMillis = currentMillis;
  Serial.print("Priming: ");
  digitalClockDisplay();
  analogWrite(pumpControl, 127);
  digitalWrite(greenLed, HIGH);
  wateringMillis = currentMillis + primeLinePeriod;
  lastCycle = primingCycle;

  // blinkMillis = currentMillis;
  // if (blinkMillis - newBlinkMillis >= blinkDelay)
  // {
  //   newBlinkMillis = currentMillis;
  //   if (blueLed == LOW)
  //   {
  //     blueLedState = HIGH;
  //   }
  //   else
  //     blueLedState = LOW;
  // }
  // digitalWrite(blueLed, blueLedState);
}

void runWateringCycle()
{
  //if (currentMillis - wateringMillis <= wateringCyclePeriod && currentMillis > wateringMillis)
  wateringMillis = currentMillis;
  Serial.print("Watering: ");
  digitalClockDisplay();
  analogWrite(pumpControl, 191);
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, HIGH);
  endMillis = currentMillis + wateringCyclePeriod;
  lastCycle = wateringCycle;
}

void endWateringCycle()
{
  //if (currentMillis >= endMillis)
  {
    Serial.print("Watering Complete: ");
    digitalClockDisplay();
    analogWrite(pumpControl, 0);
    for (int count = 0; count < ledCount; count++) // Assign the array pins their array position
      digitalWrite(allLed[count], LOW);            // set "LEDs" to LOW
    lastCycle = endCycle;
  }
}

void runCycleStart()
{
  runPrimeLine();
}
