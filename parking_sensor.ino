/*
  Parking Sensor
  Helps park your car by providing a distance indicator with a neopixel light set
  
  @author Belin Fieldson <thebelin@gmail.com>
  @copyright This example code is in the public domain.

   Original code for Ping))) example was created by David A. Mellis
   Adapted for HC-SR04 by Tautvidas Sipavicius

   The circuit:
  Rangefinder connection:
  * VCC connection of the sensor attached to +5V
  * GND connection of the sensor attached to ground
  * TRIG connection of the sensor attached to digital pin 2
  * ECHO connection of the sensor attached to digital pin 4

  Neopixel connection
  * 5V to 5V
  * GND to GND
  * Data to D10
*/
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Rangefinder Trigger
const int trigPin = 2;

// Rangefinder Echo
const int echoPin = 4;

// The ranges of the depth sensor in cm
const int maxRange = 100;
const int minRange = 0;

// The length of time between ping sequences
const int pingRate = 250;

// neopixel indicators
const int neoPixelPin = 6;
const int neoPixelCount = 10;

// the timeout length for sleeping
const int timeout = 2000;

// the variance the device can report without waking from timeout
const float variance = 0.1;

// The address of the indicator pixels
Adafruit_NeoPixel lights = Adafruit_NeoPixel(neoPixelCount, neoPixelPin, NEO_GRB + NEO_KHZ800);

void setup() {

  // Configure the ping sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Init the neopixels
  lights.begin();
  lights.show();
}

// The distance in cm to the nearest object
long cm = 0;

// the previous distance to nearest object
long prev = 0;
// The last time a ping was sent
long pingLastMillis = 0;

// The last time a distance was changed
long distLastMillis = 0;
void loop()
{
  // Only check distance every pingRate milliseconds
  if (pingLastMillis + pingRate < millis()) {
    cm = doPing();
  }
  
  // If distance has changed, change the indicators
  if (cm < prev - variance || cm > prev + variance) {
    prev = cm;
    distLastMillis = millis();
    // show a light indicator for the distance detected
    showLights(
      map(cm, minRange, maxRange, neoPixelCount, 0),
      colorFromDistance(cm));
  }

  // If the distance hasn't changed in the timeout period, turn off the lights
  if (distLastMillis + timeout < millis()) {
    showLights(0, lights.Color(0,0,0));
  }
}

int doPing () {
  // The duration of the ping
  long duration;
  
  // record the time of this ping
  pingLastMillis = millis();

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  return microsecondsToCentimeters(duration);  
}

// Show the number of lights of the specified color
void showLights(int numLights, uint32_t color) {
  // iterate all the lights
  for (int lt = 0;lt < neoPixelCount;lt++) {
    if (lt < numLights) {
      lights.setPixelColor(lt, color);
    } else {
      lights.setPixelColor(lt, 0);
    }
  }
  lights.show();
}

// Get a color from the specified distance
// assign the distance to a color and brightness:
uint32_t colorFromDistance (long distance) {
  if (distance > 50) {
    return lights.Color(0, 30, 0);
  } else if (distance > 20) {
    return lights.Color(25, 25, 0);
  } else if (distance < 5) {
    return lights.Color(255, 0, 0);
  }
  return lights.Color(100, 0, 0);
}

// Convert time spent travelling at the speed of sound into a distance
long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

