#include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <SoftwareSerial.h>

#define LED_PIN 9
#define LED_COUNT 60
#define LED_TYPE WS2812B // i think this is it?
#define COLOUR_ORDER GRB

#define BLUETOOTH_MODE 0
SoftwareSerial bluetooth(4, 3);   // RX = pin 4, TX = pin 3

CRGB leds[LED_COUNT];

int rpmEmu = 2000; 

int rightSideMax = 1; //where 1 is center (LED 30) and 29 is fully right (LED 59)
int leftSideMax = 1; // where 1 is center (LED 29) and 29 is fully left  (LED 0)

unsigned long timePrevious = 0;   // to keep track of deceleration, fake "braking" stat
int kphPrevious = 0;              
const int SAMPLE_INTERVAL = 100;  


const int rpmMax = 7000; 
const int rightSideDivisor = sq(rpmMax/100)/15;
int changeInKph = 0;


int kphEmu = 80;
int throttleEmu = 20; // 0 to 100
int engineTempEmu = 25;
int emuCmd = 0;
int displayCmd = 0;



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

char readBuffer[48];
uint8_t readLength = 0;

void lfsGetData() {
  while (Serial.available() > 0) {
    char lfsRead = Serial.read();
    if (lfsRead == '\n') {
      readBuffer[readLength] = 0;
      readLength = 0;

      char *sepA = strtok(readBuffer, ","); //data is sent as XX,XX,XX,XX. this separates it
      char *sepB = strtok(NULL, ",");
      char *sepC = strtok(NULL, ",");
      char *sepD = strtok(NULL, ",");

      if (sepA && sepB && sepC && sepD) {
        kphEmu = atof(sepA);
        rpmEmu = atol(sepB);
        engineTempEmu = atof(sepC);
        throttleEmu = atof(sepD);
      }
    } else if (readLength < sizeof(readBuffer) - 1) {
      readBuffer[readLength++] = lfsRead;
    } else {
      readLength = 0;   // overflow, discard
    }
  }
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
  EVERY_N_MILLISECONDS(100) {
  
  lfsGetData();
  rpmRead = rpmEmu;
  kphRead = kphEmu;
  engineTempRead = engineTempEmu;
  throttleRead = throttleEmu;

  kphMap = constrain(map(kphRead, 0, 120, 0, LED_COUNT - 1), 0, LED_COUNT - 1);
  rpmMap = constrain(map(rpmRead, 0, 7000, 0, LED_COUNT - 1), 0, LED_COUNT - 1);
  engineTempMap = constrain(map(engineTempRead, 0, 120, 0, LED_COUNT - 1), 0, LED_COUNT - 1);



  displayCmd = 0;



  unsigned long timeCurrent = millis();

  changeInKph = kphRead - kphPrevious;

  kphPrevious = kphRead;      //reset ready for next read
  timePrevious = timeCurrent;







  switch (displayCmd)
  {

  case 0:
    // everything!!!

    //right bar = ((throttleRead*2.5)+((sq(b)/39200000)*2))*6+30        ORIGINAL FORMULA
    //left bar = -0.015*kphRead*(1+a)                                   ORIGINAL FORMULA

    FastLED.clear();
    
    rightSideMax = 
      constrain(
        int(
        
          ((throttleRead*3)/20)        //throttle fx is limited from 0 to 15
          +((sq(rpmRead/100)/326))   //rpm fx      is limited from 0 to 15
        )
         + 30,
        30, LED_COUNT-1);

    leftSideMax = 
      constrain(
        29+int(
          -0.1*kphRead*(1-(throttleRead/100.0))
        ),0, LED_COUNT/2-1);

    leds[leftSideMax] = CRGB::Green;
    leds[rightSideMax] = CRGB::Blue;

    FastLED.show();
    break;

      
  case 1:
    
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

  //Serial.print(rpmRead);
  //Serial.println("rpm");

  //Serial.print(kphRead);
  //Serial.println("kph");
  //Serial.println("");

  // code

}
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
