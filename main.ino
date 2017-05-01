#include "FastLED.h"
#include <math.h>
/*
 HC-SR04 Ping distance sensor]
 VCC to arduino 5v GND to arduino GND
 Echo to Arduino pin 13 Trig to Arduino pin 12
 Red POS to Arduino pin 11
 Green POS to Arduino pin 10
*/

#define NUM_LEDS 60

#define DATA_PIN 7

// Define the array of leds
CRGB leds[NUM_LEDS];

// Module Pins
#define trigPin 13
#define echoPin 12

// Thresholds (in CM)
long SECTION_SIZE = 30;
long STOP_DISTANCE = 40;
long WARNING_THRESHOLD = 30;
long MAX_THRESHOLD = 275;

long RED_THRESHOLD = STOP_DISTANCE;
long YELLOW_THRESHOLD = RED_THRESHOLD + SECTION_SIZE;
long GREEN_THRESHOLD = YELLOW_THRESHOLD + SECTION_SIZE;

const int numReadings = 5;
 
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

void setup() {
  Serial.begin (9600);

  // Fast LED Setup
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // FastLED.setBrightness(84);

  // HC-SR04 Setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {
  long distance, lastRun;
  bool red, yellow, green;
  total = total - readings[readIndex];
  readings[readIndex] = calculateDistance();
  total = total + readings[readIndex];
  
  readIndex++;
  if (readIndex >= numReadings) readIndex = 0;

  distance = abs(total / numReadings);

  yellow = distance <= YELLOW_THRESHOLD && distance > RED_THRESHOLD;
  red = distance <= RED_THRESHOLD;
  green = distance > YELLOW_THRESHOLD && distance <= GREEN_THRESHOLD;

  if (red) {
    setRed();
  } else if (yellow) {
    setYellow(distance);
  } else if (green) {
    setGreen(distance);
  } else {
    setGreenStandby();
  }

  

  Serial.print(distance);
  Serial.println(" cm");
  FastLED.show();
  delay(1);
}

void lightItUp(const CRGB& pColor, const CRGB& sColor, uint16_t travel) {
  uint16_t travelFix = travel * (NUM_LEDS/SECTION_SIZE);
  uint16_t leftStop = travelFix/2;
  uint16_t rightStop = NUM_LEDS - leftStop;
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < leftStop || i > rightStop) {
      leds[i] = pColor;
    } else {
      leds[i] = sColor;
    }
  }

  FastLED.show();
}


long calculateDistance() {
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  return (duration/2) / 29.1;
}

void setGreenStandby() {
  return fill_solid(leds, NUM_LEDS, CRGB::Green);
}

void setRed() {
  return fill_solid(leds, NUM_LEDS, CRGB::Red);
}

void setGreen(long distance) {
  long travel = GREEN_THRESHOLD - distance;
  return lightItUp(CRGB::Yellow, CRGB::Green, travel);
}

void setYellow(long distance) {
  long travel = YELLOW_THRESHOLD - distance;
  return lightItUp(CRGB::Red, CRGB::Yellow, travel);  
}

