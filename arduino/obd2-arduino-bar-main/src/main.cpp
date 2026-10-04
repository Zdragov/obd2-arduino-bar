#include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <SoftwareSerial.h>

#define LED_PIN 9
#define LED_COUNT 60
#define LED_TYPE WS2812B // i think this is it?
#define COLOUR_ORDER GRB

#define BLUETOOTH_MODE 0
SoftwareSerial bluetooth(4, 3); // RX = pin 4, TX = pin 3

CRGB leds[LED_COUNT];

int rpmEmu = 2000;

int rightSideMax = 1; // where 1 is center (LED 30) and 29 is fully right (LED 59)
int leftSideMax = 1;  // where 1 is center (LED 29) and 29 is fully left  (LED 0)

unsigned long timePrevious = 0; // to keep track of deceleration, fake "braking" stat
float kphPrevious = 0;
const int SAMPLE_INTERVAL = 100;

const int rpmMax = 7000;
const int rightSideDivisor = sq(rpmMax / 100) / 15;
CRGB rightSideFillColour = CRGB::Blue;
CRGB leftSideFillColour = CRGB::Green;

float changeInKph = 0;

float kphEmu = 80;
int throttleEmu = 20; // 0 to 100
int engineTempEmu = 25;;
int displayCmd = 0;

int rpmRead = rpmEmu;
float kphRead = kphEmu;
int throttleRead = throttleEmu;
int engineTempRead = engineTempEmu;


char readBuffer[48];
uint8_t readLength = 0;

void lfsGetData()
{
  while (Serial.available() > 0)
  {
    char lfsRead = Serial.read();
    if (lfsRead == '\n')
    {
      readBuffer[readLength] = 0;
      readLength = 0;

      char *sepA = strtok(readBuffer, ","); // data is sent as XX,XX,XX,XX. this separates it
      char *sepB = strtok(NULL, ",");
      char *sepC = strtok(NULL, ",");
      char *sepD = strtok(NULL, ",");

      if (sepA && sepB && sepC && sepD)
      {
        kphEmu = atof(sepA);
        rpmEmu = atof(sepB);
        engineTempEmu = atof(sepC);
        throttleEmu = atof(sepD);
      }
    }
    else if (readLength < sizeof(readBuffer) - 1)
    {
      readBuffer[readLength++] = lfsRead;
    }
    else
    {
      readLength = 0; // overflow, discard
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
  EVERY_N_MILLISECONDS(100)
  {

    lfsGetData();
    rpmRead = rpmEmu;
    kphRead = kphEmu;
    engineTempRead = engineTempEmu;
    throttleRead = throttleEmu;

    displayCmd = 0;

    unsigned long timeCurrent = millis();

    changeInKph = kphRead - kphPrevious;

    kphPrevious = kphRead; // reset ready for next read
    timePrevious = timeCurrent;

    switch (displayCmd)
    {

    case 0:
      // everything!!!

      // right bar = ((throttleRead*2.5)+((sq(b)/39200000)*2))*6+30        ORIGINAL FORMULA
      // left bar = -0.015*kphRead*(1+a)                                   ORIGINAL FORMULA

      FastLED.clear();

      rightSideMax =
          constrain(
              int(
                  ((throttleRead * 3) / 20)                  // throttle fx is limited from 0 to 15
                  + ((sq(rpmRead / 100) / rightSideDivisor)) // rpm fx      is limited from 0 to 15
                  ) +
                  30,
              30, LED_COUNT - 1);

      leftSideMax =
          constrain(
              29 + int(
                       (constrain(changeInKph, -5, 0)*15)),
              0, LED_COUNT / 2 - 1);

      leds[leftSideMax] = CRGB::Green;
      leds[rightSideMax] = CRGB::Blue;



      // for right side, passive colour will be blue. active colour will be white/red tip
      if (throttleRead >= 70) {
          rightSideFillColour = CRGB(255,255,255);         
      } else if (throttleRead > 40) {
          uint8_t v = (uint8_t)((throttleRead - 40)*255L/30);
          rightSideFillColour = CRGB(v,v,255);
      } else {
          rightSideFillColour = CRGB(0,0,255);
      }

      fill_solid(&leds[30], rightSideMax-29, rightSideFillColour);

      for (int i=0;i<6;i++) {
        int curLED = rightSideMax-i;
        if (curLED < 30) {break;}
        uint8_t alpha = 255 - (i*51);
        leds[curLED] = blend(leds[curLED], CRGB(255,0,0), alpha);
      }

      

      // for left side, passive colour will be green, active colour is undetermined (white temp)
      if (changeInKph > -0.6) {
        leftSideFillColour = CRGB(0,255,0);
      } else if (changeInKph > -2.0) {
          // scale by 100 to avoid float 
          // -60 = threshold, -200 = full white
          int v = (int)(-changeInKph * 100 - 60) * 255 / 140;
          v = constrain(v, 0, 255);
          leftSideFillColour = CRGB(v, 255, v);
      } else {
        leftSideFillColour = CRGB(255,255,255);
      }

      fill_solid(&leds[leftSideMax], 30-leftSideMax, leftSideFillColour);

      FastLED.show();
      break;


    }

    // Serial.print(rpmRead); Serial.println("rpm");Serial.print(kphRead);Serial.println("kph");Serial.println("");

    // code
  }
}

