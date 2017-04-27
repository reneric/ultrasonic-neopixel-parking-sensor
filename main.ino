#include "FastLED.h"
#include <math.h>
/*
 HC-SR04 Ping distance sensor]
 VCC to arduino 5v GND to arduino GND
 Echo to Arduino pin 13 Trig to Arduino pin 12
 Red POS to Arduino pin 11
 Green POS to Arduino pin 10
*/

#define NUM_LEDS 16

#define DATA_PIN 7
#define CLOCK_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];


// Module Pins
#define trigPin 13
#define echoPin 12

// LED Pins
DATA_PIN = 14

// Thresholds (in CM)
#define RED_THRESHOLD 90
#define YELLOW_THRESHOLD 150
#define GREEN_THRESHOLD 210
#define MAX_THRESHOLD 275

void setup() {
  Serial.begin (9600);

  // Fast LED Setup
  LEDS.addLeds<WS2812,DATA_PIN,RGB>(leds,NUM_LEDS);
  LEDS.setBrightness(84);

  // HC-SR04 Setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  long duration, distance;
  bool red, yellow, green, outOfRange;
  
  distance = calculateDistance();

  outOfRange = distance >= MAX_THRESHOLD || distance <= 0;
  yellow = distance <= YELLOW_THRESHOLD && distance > RED_THRESHOLD;
  red = distance <= RED_THRESHOLD;
  green = distance > YELLOW_THRESHOLD && distance <= GREEN_THRESHOLD;

  if (red) {
    setRed();
  } else if (yellow) {
    setYellow();
  } else if (green) {
    setGreen();
  } else {
    Serial.println("Out of range");
    setGreenStandby();
  }

  Serial.print(distance);
  Serial.println(" cm");
  FastLED.show();
  delay(10);
}

int calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  return (duration/2) / 29.1;
}

void setGreenStandby(long distance) {
  Serial.println("setGreenStandby");
  fill_solid(leds, NUM_LEDS, CRGB::Green, 1);
}

void setRed(long distance) {
  Serial.println("setRed");
  long percentage = distance - RED_THRESHOLD;
  lightItUp(CRGB::Red, CRGB::Yellow, percentage)
}

void setGreen(long distance) {
  Serial.println("setGreen");
  long percentage = distance - GREEN_THRESHOLD;
  fill_solid(leds, NUM_LEDS, CRGB::Green, percentage);
}

void setYellow(long distance) {
  Serial.println("setYellow");
  long percentage = distance - YELLOW_THRESHOLD;
  lightItUp(CRGB::Yellow, CRGB::Green, percentage);
}

void lightItUp(const primaryColor, const secondaryColor, uint16_t percentage) {
  uint16_t leftStop = ceil(percentage/2);
  uint16_t rightStop = NUM_LEDS - leftStop;
  Serial.println("lightItUp");
  Serial.print("leftStop");
  Serial.println(leftStop);
  Serial.print("rightStop");
  Serial.println(rightStop);
  // Left
  for (int i = 0; i < level; i++) {
    leds[i] = primaryColor;
  }
  
  // Right
  for (int i = NUM_LEDS; i > rightStop; --i) {
    leds[i] = primaryColor;
  }

  // Middle
  for (int i = level; i <= rightStop; i++) {
    leds[i] = secondaryColor;
  }
}

