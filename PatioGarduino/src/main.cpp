#include <Arduino.h>
#include <TimeLib.h>
#include <DS1307RTC.h> // a basic DS1307 library that returns time as a time_t
#include <Wire.h>
#include <TimeAlarms.h>
#include <Bounce2.h>

//#include <SNTPtime.h>

//pins
const int pumpControl = 10;
const int greenButtonPin = 12;
const int redButtonPin = 11;
const int blueLed = 4;

//Button management
int greenButtonState;                    // the current reading from the input pin
int lastGreenButtonState = HIGH;         // the previous reading from the input pin
int redButtonState;                      // the current reading from the input pin
int lastRedButtonState = HIGH;           // the previous reading from the input pin
unsigned long lastDebounceTimeGreen = 0; // the last time the output pin was toggled
unsigned long lastDebounceTimeRed = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;        // the debounce time; increase if the output flickers

//Watering Cycle Timing
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

//blink
const unsigned long blinkDelay = 500;
unsigned long blinkMillis = 0;
unsigned long newBlinkMillis = 0;
int blueLedState;

//Functions

void blinkBlueLed()
{
  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long newBlinkMillis = millis();

  if (newBlinkMillis - blinkMillis >= blinkDelay)
  {
    // save the last time you blinked the LED
    newBlinkMillis = millis();

    // if the LED is off turn it on and vice-versa:
    if (blueLedState == LOW)
    {
      blueLedState = HIGH;
    }
    else
    {
      blueLedState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(blueLed, blueLedState);
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
  blinkBlueLed();
  Serial.print("Priming: ");
  digitalClockDisplay();
  analogWrite(pumpControl, 127);
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
  endMillis = currentMillis + wateringCyclePeriod;
  lastCycle = wateringCycle;
  digitalWrite(blueLed, HIGH);
}

void endWateringCycle()
{
  //if (currentMillis >= endMillis)
  {
    Serial.print("Watering Complete: ");
    digitalClockDisplay();
    analogWrite(pumpControl, 0);
    digitalWrite(blueLed, LOW);
    lastCycle = endCycle;
    digitalWrite(blueLed, LOW);
  }
}

void runCycleStart()
{
  runPrimeLine();
}

// the setup routine runs once when you press reset:
void setup()
{
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  setSyncProvider(RTC.get); // the function to get the time from the RTC

  //pinModes
  pinMode(pumpControl, OUTPUT);
  pinMode(greenButtonPin, INPUT_PULLUP);
  pinMode(redButtonPin, INPUT_PULLUP);
  pinMode(blueLed, OUTPUT);

  digitalWrite(pumpControl, LOW);
  digitalWrite(blueLed, LOW);
  endMillis = 0;
  primingMillis = 0;
  wateringMillis = 0;
  Serial.println("Powered On");
  lastCycle = poweredOn;

  //time_t t = now();
  //Alarm.timerOnce(10, runCycleStart);
}

void loop()
{
  // read the state of the switch into a local variable:
  int greenReading = digitalRead(greenButtonPin);
  int redReading = digitalRead(redButtonPin);
  currentMillis = millis();
  Alarm.delay(0);

  //check green button
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (greenReading != lastGreenButtonState)
  {
    // reset the debouncing timer
    lastDebounceTimeGreen = millis();
  }
  if ((millis() - lastDebounceTimeGreen) > debounceDelay)
  {
    // whatever the greenReading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (greenReading != greenButtonState)
    {
      greenButtonState = greenReading;
      // only toggle the LED if the new button state is HIGH
      if (greenButtonState == HIGH)
      {
        runCycleStart();
      }
    }
  }
  // save the greenReading. Next time through the loop, it'll be the lastGreenButtonState:
  lastGreenButtonState = greenReading;

  //check red button
  if (redReading != lastRedButtonState)
  {
    // reset the debouncing timer
    lastDebounceTimeRed = millis();
  }
  if ((millis() - lastDebounceTimeRed) > debounceDelay)
  {
    // whatever the rednReading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (redReading != redButtonState)
    {
      redButtonState = redReading;
      if (redButtonState == HIGH && lastCycle != endCycle)
      {
        endWateringCycle();
      }
    }
  }
  // save the redReading. Next time through the loop, it'll be the lastRedButtonState:
  lastRedButtonState = redReading;

  if (lastCycle == cycleStart)
    runPrimeLine();
    newBlinkMillis = currentMillis;

  
  if (lastCycle == primingCycle && currentMillis - wateringMillis <= wateringCyclePeriod && currentMillis > wateringMillis)
  {
    runWateringCycle();
  }

  if (lastCycle == wateringCycle && currentMillis >= endMillis)
  {
    endWateringCycle();
  }
}
