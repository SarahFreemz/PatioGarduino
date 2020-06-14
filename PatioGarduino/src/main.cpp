
#include <TimeLib.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <Wire.h>
#include <TimeAlarms.h>
#include <Bounce2.h>



//#include <SNTPtime.h>




//pins
int pumpControl = 10;
int greenButtonPin = 12;
int redButtonPin = 11;

unsigned long currentMillis;
unsigned long endMillis;  //some global variables available anywhere in the program
unsigned long primingMillis;
unsigned long wateringMillis;
const unsigned long primeLinePeriod = 5000;  //prime water line at 6v for 10sec
const unsigned long wateringCyclePeriod = 5000;  //run watering cycle at 9v for 3min

int greenButtonState;             // the current reading from the input pin
int lastGreenButtonState = HIGH;   // the previous reading from the input pin
int redButtonState;             // the current reading from the input pin
int lastRedButtonState = HIGH;   // the previous reading from the input pin
unsigned long lastDebounceTimeGreen = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTimeRed = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

int lastCycle;
const int poweredOn = 1;
const int cycleStart = 2;
const int primingCycle = 3;
const int wateringCycle = 4;
const int endCycle = 5;



// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  
  pinMode(pumpControl, OUTPUT);
  pinMode(greenButtonPin, INPUT_PULLUP);
  pinMode(redButtonPin, INPUT_PULLUP);
  
  digitalWrite(pumpControl, LOW);
  endMillis = 0;
  primingMillis = 0;   
  wateringMillis = 0;
  Serial.println("Powered On");
  lastCycle = poweredOn;

  time_t t = now();
  Alarm.timerOnce(10, runCycleStart);
}




void loop() {
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
  if (greenReading != lastGreenButtonState) {
    // reset the debouncing timer
    lastDebounceTimeGreen = millis();
  }
  if ((millis() - lastDebounceTimeGreen) > debounceDelay) {
    // whatever the greenReading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (greenReading != greenButtonState) {
      greenButtonState = greenReading;
      // only toggle the LED if the new button state is HIGH
      if (greenButtonState == HIGH) {
          runCycleStart();}
    }
  }
  // save the greenReading. Next time through the loop, it'll be the lastGreenButtonState:
  lastGreenButtonState = greenReading;


//check red button
  if (greenReading != lastGreenButtonState) {
    // reset the debouncing timer
    lastDebounceTimeGreen = millis();
  }
  if ((millis() - lastDebounceTimeGreen) > debounceDelay) {
    // whatever the greenReading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (greenReading != greenButtonState) {
      greenButtonState = greenReading;
      // only toggle the LED if the new button state is HIGH
      if (greenButtonState == HIGH) {
          endWateringCycle();}
    }
  }
  // save the rReading. Next time through the loop, it'll be the lastRedButtonState:
  lastRedButtonState = redReading;

if (lastCycle == cycleStart) runPrimeLine();

if (lastCycle == primingCycle && currentMillis - wateringMillis <= wateringCyclePeriod && currentMillis > wateringMillis) {
      runWateringCycle();
}

if (lastCycle == wateringCycle &&currentMillis >= endMillis) {
      endWateringCycle();
}
  
 

}


void runCycleStart (){
  runPrimeLine();
}

void runPrimeLine() {
  //if (currentMillis - primingMillis <= primeLinePeriod){
      primingMillis = currentMillis;
      Serial.print("Priming: ");
       digitalClockDisplay();
     analogWrite(pumpControl, 127);
      wateringMillis = currentMillis + primeLinePeriod;
      lastCycle = primingCycle;
}

void runWateringCycle() {
  //if (currentMillis - wateringMillis <= wateringCyclePeriod && currentMillis > wateringMillis) 
  {
      wateringMillis = currentMillis;
      Serial.print("Watering: ");
      digitalClockDisplay();
      analogWrite(pumpControl, 191);
      endMillis = currentMillis + wateringCyclePeriod;
      lastCycle = wateringCycle;
      }
}

void endWateringCycle() {
  //if (currentMillis >= endMillis) 
  {
      Serial.print("Watering Complete: ");
      digitalClockDisplay();
      analogWrite(pumpControl, 0);
      lastCycle = endCycle;}
}

void digitalClockDisplay(){
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

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
