/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/
#include <Arduino.h>

int pumpControl = 10;
unsigned long endMillis;  //some global variables available anywhere in the program
unsigned long primingMillis;
unsigned long wateringMillis;
const unsigned long primeLinePeriod = 5000;  //prime water line at 6v for 10sec
const unsigned long wateringCyclePeriod = 5000;  //run watering cycle at 9v for 3min

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(pumpControl, OUTPUT);
  analogWrite(pumpControl, 0);
  endMillis = 0;
  primingMillis = 0;
  wateringMillis = 0;
}

// the loop routine runs over and over again forever:
void loop() {
  unsigned long currentMillis = millis();

 
    //prime water line at 6v for 10sec
    primingMillis = currentMillis;
  if (currentMillis < primingMillis + primeLinePeriod && currentMillis > primingMillis ) {
    wateringMillis = currentMillis + primeLinePeriod;
    analogWrite(pumpControl, 127);
    Serial.println("Priming");
     }

    //run watering cycle at 9v for 3min
  if (currentMillis > wateringMillis && currentMillis <= wateringMillis + wateringCyclePeriod) {
      endMillis = currentMillis + wateringCyclePeriod;
      analogWrite(pumpControl, 191);
      Serial.println("Watering");
      }

     // turn pump off
  if (currentMillis >= wateringMillis + wateringCyclePeriod && currentMillis > endMillis) {
      analogWrite(pumpControl, 0);
      primingMillis = currentMillis;
      Serial.println("Watering Complete");
     }
 
  }
  //fill water line at 6v for 10sec
/*  analogWrite(pumpControl, 127);
  delay(10000);        // delay in between reads for stability
  //water at 9v for 3min
  analogWrite(pumpControl, 191);
  delay(180000);
  analogWrite (pumpControl, 0);*/
