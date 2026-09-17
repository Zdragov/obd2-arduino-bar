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
int throttleEmu = 0.3; //min 0, max 1



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

  //mock and test data
  rpmRead = rpmEmu;
  kphRead = kphEmu;


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