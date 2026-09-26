#include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#define LED_PIN 9
#define LED_COUNT 60
#define LED_TYPE WS2812B // i think this is it?
#define COLOUR_ORDER GRB

CRGB leds[LED_COUNT];

int rpmEmu = 2000;
int kphEmu = 80;
int throttleEmu = 20; // 0 to 100
int engineTempEmu = 25;
int emuCmd = 0;
int displayCmd = 0;

String lfsData = "";

int emuCruiseAccelerating = 1;

int rpmRead = rpmEmu;
int kphRead = kphEmu;
int throttleRead = throttleEmu;
int engineTempRead = engineTempEmu;

int emuInfoSet(int cmdSent, int spd, int rpm, int throttle, int engineTemp)
{
  emuCmd = cmdSent;
  kphEmu = 100;
  rpmEmu = 2250;
  throttleEmu = 20;
  engineTempEmu = 90;
}

int rpmMap;
int kphMap;
int engineTempMap;

int lightSensor = 50;

// Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void lfsGetData() {
  while (Serial.available() > 0) {
    char lfsRead = Serial.read();

    if (lfsRead == '\n') { //
      int separator1 = lfsData.indexOf(','); //separators find the commas in the LFS data and separate them. finds first comma and notes down location
      int separator2 = lfsData.indexOf(',', separator1 + 1); //finds second comma and notes down location
      int separator3 = lfsData.indexOf(',', separator2 + 1); //finds third comma and notes down location
      
      if (separator1 > 0 && separator2 > 0 && separator3 > 0) { //if these separators exist, continue onwards
        kphEmu = lfsData.substring(0, separator1).toFloat(); //if the data is 10,13,50    this will cut out 10 out of that string
        rpmEmu = lfsData.substring(separator1+1,separator2).toFloat();
        engineTempEmu = lfsData.substring(separator2+1,separator3).toFloat();
        throttleEmu = lfsData.substring(separator3+1).toFloat();

      }

      lfsData = ""; //reset the Data
    } else {
      lfsData = lfsData + lfsRead;
    }
  } // closes serial
}


void setup()
{
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

}

void loop()
{

  // mock and test data, and serial commands

  lfsGetData();
  rpmRead = rpmEmu;
  kphRead = kphEmu;

  kphMap = constrain(map(kphRead, 0, 120, 0, LED_COUNT - 1), 0, LED_COUNT - 1);
  rpmMap = constrain(map(rpmRead, 0, 7000, 0, LED_COUNT - 1), 0, LED_COUNT - 1);
  engineTempMap = constrain(map(engineTempRead, 0, 120, 0, LED_COUNT - 1), 0, LED_COUNT - 1);

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

  if (Serial.available() != 0)
  {
    char receivedCmd = Serial.read();

    switch (receivedCmd)
    {
    case 'a':
      emuCmd = 1;
      Serial.println("Slowly Accelerating...");
      kphEmu = 0;
      rpmEmu = 2000;
      engineTempEmu = 90;
      break;
    case 'b':
      emuCmd = 2;
      Serial.println("Braking...");
      kphEmu = 100;
      rpmEmu = 2000;
      throttleEmu = 1;
      engineTempEmu = 90;
      break;
    case 'c':
      emuCmd = 3;
      Serial.println("Cruising (95-105kph)...");
      kphEmu = 100;
      rpmEmu = 2250;
      throttleEmu = 20;
      engineTempEmu = 90;
      break;
    case 'd':
      emuCmd = 4;
      Serial.println("Quickly Accelerating...");
      kphEmu = 0;
      rpmEmu = 2000;
      engineTempEmu = 90;
      break;
    case 'e':
      emuCmd = 5;
      Serial.println("Cold starting...");
      kphEmu = 0;
      rpmEmu = 2650;
      throttleEmu = 5;
      engineTempEmu = 25;
      break;

    case '1':
      displayCmd = 1;
      Serial.println("Showing RPM");
      break;
    case '2':
      displayCmd = 2;
      Serial.println("Showing KPH");
      break;
    case '3':
      displayCmd = 3;
      Serial.println("Showing Temp");
      break;
    default:
      break;
    }
  }

//LFS data goes here




  switch (displayCmd)
  {
  case 1:
    Serial.print("abcd");
    // FastLED.clear();
    // RPM display start

    // if RPM is increasing...
    fadeToBlackBy(leds, LED_COUNT, 70);
    // else if RPM is decreasing...
    // fadeToBlackBy(leds, LED_COUNT,150);

    leds[rpmMap] = CHSV(0, 255, 255);

    FastLED.show();

    // RPM display end
    break;

  case 2:
    // Speedometer display Start

    FastLED.clear();

    fill_solid(&leds[0], kphMap, CRGB(120, 120, 120));

    leds[10] = CRGB::Yellow; // 20kph
    leds[20] = CRGB::Yellow; // 40kph
    leds[25] = CRGB::Green;  // 50kph
    leds[30] = CRGB::Yellow; // 60kph
    leds[40] = CRGB::Yellow; // 80kph
    leds[50] = CRGB::Green;  // 100kph

    FastLED.show();

    // Speedometer display End
    break;

  case 3:

    FastLED.clear();

    // Water Temp display Start (Only this is displayed on startup. Disappears when speed goes above 10kph)

    // meter goes from 20 to 120
    // 20-40c = too cold
    // 40-70c = cold but OK
    // 70-100c = OK
    // 100-120c = too hot

    // if engine temp is detected,
    fill_solid(&leds[0], 12, CHSV(180, 255, 100));
    fill_solid(&leds[12], 18, CHSV(250, 255, 100));
    fill_solid(&leds[45], 15, CHSV(0, 255, 100));

    leds[engineTempMap] = CHSV(0, 0, 100);

    if (engineTempRead > 105)
    {
      // flash red zone
    }

    FastLED.show();

    break;

    // Water Temp display End
  }

  // compile every layer into one

  // serial print

  Serial.print(rpmRead);
  Serial.println("rpm");

  Serial.print(kphRead);
  Serial.println("kph");
  Serial.println("");

  // code

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

// ctrl shift i to auto indent




/*

TODO

Half light brightness when in between values'

Finalize view

*/