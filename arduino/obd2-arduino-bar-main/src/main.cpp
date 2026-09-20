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
int throttleEmu = 20; //0 to 100
int emuCmd = 0;

int emuCruiseAccelerating = 1;



int rpmRead = rpmEmu;
int kphRead = kphEmu;

int rpmMap;
int kphMap;
int throttleRead = throttleEmu;

int lightSensor = 50;




//Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);



void setup() {
  // put your setup code here, to run once:

  Serial.begin(38400);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOUR_ORDER>(leds, LED_COUNT);
  FastLED.setBrightness(16);

  fill_solid(leds, LED_COUNT, CRGB::Red);
  FastLED.show();
  delay(333);


  fill_solid(leds, LED_COUNT, CRGB::Blue);
  FastLED.show();
  delay(333);


  fill_solid(leds, LED_COUNT, CRGB::Green);
  FastLED.show();
  delay(333);

  FastLED.clear();
  FastLED.show();
  delay(333);


  //int rpmDisplay(
    //display rpm
  //)

}

void loop() { 

  //mock and test data, and serial commands
  rpmRead = rpmEmu;
  kphRead = kphEmu;

/*
  if (rpmEmu < 7000) {
    rpmEmu = rpmEmu + 100;
  } else {
    rpmEmu = 2000;
  }
  
  if (kphEmu < 100) {
    kphEmu = kphEmu + 2;
  } else {
    kphEmu = 0;
  }

*/





  if (Serial.available() != 0) {
    char receivedCmd = Serial.read();

    switch (receivedCmd) {
      case 'a':
        emuCmd = 1;
        Serial.println("Slowly Accelerating...");
        kphEmu = 0;
        rpmEmu = 2000;
        break;
      case 'b':
        emuCmd = 2;
        Serial.println("Braking...");
        kphEmu = 100;
        rpmEmu = 2000;
        throttleEmu = 1;
        break;
      case 'c':
        emuCmd = 3;
        Serial.println("Cruising (95-105kph)...");
        kphEmu = 100;
        rpmEmu = 2250;
        throttleEmu = 20;
        break;
      case 'd':
        emuCmd = 4;
        Serial.println("Quickly Accelerating...");
        kphEmu = 0;
        rpmEmu = 2000;
        break;
      default:
        break;
    }

  }

  if (emuCmd == 1) { // Emulate casual acceleration
    
    if (kphEmu < 100) {
      rpmEmu = rpmEmu + 100;
      kphEmu = kphEmu + 2;
      if (rpmEmu > 3000) {
        rpmEmu = 2000;
      }
      throttleEmu = 50;
    } else {
      kphEmu = 100;
      rpmEmu =  2250;
      throttleEmu = 20;
    } 
  } else if (emuCmd == 2) { // Emulate braking from 100kph
    if (kphEmu > 0) {
      rpmEmu = 1750;
      kphEmu = kphEmu - 2;
    }
  } else if (emuCmd == 3) { // Emulate speed bouncing from 95kph to 105kph
    if (emuCruiseAccelerating == 1) {
      kphEmu = kphEmu + 1;
      rpmEmu = rpmEmu + 50;
      throttleEmu = 23;
      if (kphEmu > 105) {
        emuCruiseAccelerating = 0;
      }
    } else if (emuCruiseAccelerating == 0) {
      kphEmu = kphEmu - 1;
      rpmEmu = rpmEmu - 50;
      throttleEmu = 17;
      if (kphEmu < 95) {
        emuCruiseAccelerating = 1;
      }
    }
  } else if (emuCmd == 4) { // Emulate hard acceleration
    if (kphEmu < 100) {
      rpmEmu = rpmEmu + 500;
      kphEmu = kphEmu + 5;
      if (rpmEmu > 7000) {
        rpmEmu = 4000;
      }
      throttleEmu = 90;
    } else {
      kphEmu = 100;
      rpmEmu =  2250;
      throttleEmu = 20;
    } 
  }



  //serial print


  Serial.print(rpmRead);
  Serial.println("rpm");

  Serial.print(kphRead);
  Serial.println("kph");
  Serial.println("");



  //code

  

  

  kphMap = constrain(map(kphRead, 0, 120, 0, LED_COUNT-1), 0, LED_COUNT-1);
  rpmMap = constrain(map(rpmRead, 0, 7000, 0, LED_COUNT-1), 0, LED_COUNT-1);

  //fill_solid(&leds[0], kphMap, CRGB(120,120,120)); Speed Bar

  //RPM display start

  //if RPM is increasing...
  fadeToBlackBy(leds, LED_COUNT,70);
  //else if RPM is decreasing...
  fadeToBlackBy(leds, LED_COUNT,150);

  leds[rpmMap] = CHSV(0, 255, 255);
    
  FastLED.show();

  //RPM display end

  //Speedometer display Start

  FastLED.clear();

  fill_solid(&leds[0], kphMap, CRGB(120,120,120));

  leds[10] = CRGB::Yellow; //20kph
  leds[20] = CRGB::Yellow; //40kph
  leds[25] = CRGB::Green; //50kph
  leds[30] = CRGB::Yellow; //60kph
  leds[40] = CRGB::Yellow; //80kph
  leds[50] = CRGB::Green; //100kph

  FastLED.show();

  //Speedometer display End

  //Water Temp display Start (Only this is displayed on startup. Disappears when speed goes above 10kph)

  engineTempMap = map(engineTempRead, 0, 120, 0, LED_COUNT-1);

  
  //meter goes from 20 to 120
  //20-40c = too cold
  //40-70c = cold but OK
  //70-100c = OK
  //100-120c = too hot

  //if engine temp is detected,
  fill_solid(&leds[0], 12, CHSV(180,100,100));
  fill_solid(&leds[12], 18, CHSV(250,100,100));
  fill_solid(&leds[45], 15, CHSV(0,100,100));

  leds[engineTempMap] = CHSV(0,0,100);

  if (engineTempRead > 105){
    //flash red zone
  }

  //Water Temp display End



  delay(300);
}




/*

Fill Entire Strip

Fill all LEDs with a single color:

// Fill entire strip
fill_solid(leds, NUM_LEDS, CRGB::Blue);

Fill a Range

Fill a specific section of your strip:

// Fill range
fill_solid(&leds[10], 20, CRGB::Green);  // LEDs 10-29

This fills 20 LEDs starting from index 10, effectively controlling LEDs 10 through 29.
Fill with HSV

Use HSV color space for more intuitive color selection:

// Fill with HSV
fill_solid(leds, NUM_LEDS, CHSV(160, 255, 255));

*/