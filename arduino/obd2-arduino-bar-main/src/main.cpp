#include <Arduino.h>
//#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#define LED_PIN 9
#define LED_COUNT 60
#define LED_TYPE WS2812B //i think this is it?
#define COLOUR_ORDER GRB

CRGB leds[LED_COUNT];

int rpmEmu = 2000;
int kphEmu = 80;
int throttleEmu = 30; //in %



int rpmRead = rpmEmu;
int kphRead = kphEmu;
int throttleRead = throttleEmu;

int lightSensor = 50;




//Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);



void setup() {
  // put your setup code here, to run once:

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOUR_ORDER>(leds, LED_COUNT);
  FastLED.setBrightness(16);

  fill_solid(leds, LED_COUNT, CRGB::Red);
  FastLED.show();
  delay(1000);


  fill_solid(leds, LED_COUNT, CRGB::Blue);
  FastLED.show();
  delay(1000);


  fill_solid(leds, LED_COUNT, CRGB::Green);
  FastLED.show();
  delay(1000);


  //int rpmDisplay(
    //display rpm
  //)

}

void loop() { /*
  // put your main code here, to run repeatedly:

  switch (displayChoice) { //somehow, find a way to not call this every loop
    case rpm:
      rpmDisplay();
      break;
    case speed:
      speedDisplay();
      break;
  }
*/
  delay(100);

  if (rpmEmu < 7000) {
    rpmEmu = rpmEmu + 100
  } else {
    rpmEmu = 0;
  }
}

