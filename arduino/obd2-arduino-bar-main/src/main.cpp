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

  FastLED.clear();

  

  kphMap = map(kphRead, 0, 120, 0, 60);
  rpmMap = map(rpmRead, 0, 7000, 0, 60);

  //fill_solid(&leds[0], kphMap, CRGB(120,120,120)); Speed Bar

  //currently developing... RPM display
  leds[rpmMap] = CRGB::Red;
 



  FastLED.show();


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