/* CirqueIT, LLC. May 2018
  LED Contortion Act
  This is a preliminary program which controls the brightness of
  300 leds on a strip with the data read from a Time of Flight sensor.
  It iterates through different color palettes and animations
  from the FAST LEDs Library Example Patch. */

#include <FastLED.h>
#include <pixeltypes.h>
#include <SparkFun_RFD77402_Arduino_Library.h> //Use Library Manager or download here: https://github.com/sparkfun/SparkFun_RFD77402_Arduino_Library
RFD77402 distance_sensor; //Hook object to the library

#define LED_PIN     5
#define NUM_LEDS    300
#define BRIGHTNESS  50
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


#define UPDATES_PER_SECOND 100
#define PLATE_THRESHOLD 500

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
  CRGB::Red,
  CRGB::Gray, // 'white' is too bright compared to red and blue
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Red,
  CRGB::Gray,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Blue,
  CRGB::Black,
  CRGB::Black
};

#define POWER_UP_DELAY_MILLIS 3000
#define DATA_RATE_BAUD        9600

int intensity = 255;
unsigned int distance = 0;

int plate0 = 0;
int plate1 = 0;
int plate2 = 0;

int rgb[3];
int color_selector = 0;

void setup() {
  delay(POWER_UP_DELAY_MILLIS);
  Serial.begin(DATA_RATE_BAUD);
  while (!Serial);
  Serial.println("RFD77402 Read Example:");

  if (distance_sensor.begin() == false)
  {
    Serial.println("Sensor failed to initialize. Check wiring.");
    while (1); //Freeze!
  }
  Serial.println("Sensor online!");

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}


void loop() {
  distance_sensor.takeMeasurement();

  plate0 = analogRead(A0);
  plate1 = analogRead(A1);
  plate2 = analogRead(A2);

  Serial.println(plate0);
  if (plate0 < PLATE_THRESHOLD) {
    Serial.println("Changing color.");
    color_selector = (color_selector + 1) % 3;
  }
  if (plate1 < PLATE_THRESHOLD) {
    Serial.println("Decreasing color.");
    rgb[color_selector] = constrain(rgb[color_selector] - 1, 0, 255);
  }
  if (plate2 < PLATE_THRESHOLD) {
    Serial.println("Increasing color.");
    rgb[color_selector] = constrain(rgb[color_selector] + 1, 0, 255);
  }
  
  FillLEDsFromPaletteColors();
  unsigned int distance = distance_sensor.getDistance(); //Retrieve the distance value
  uint8_t brightness = map(distance, 75, 2045, 0, 255); //may need to adjust parameters 2 and 3 to find appropriate range
  FastLED.setBrightness(brightness);
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(rgb[0], rgb[1], rgb[2]);
  }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically() {
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if ( lastSecond != secondHand) {
    lastSecond = secondHand;
    if ( secondHand ==  0)  {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 10)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if ( secondHand == 15)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 20)  {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 25)  {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 30)  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if ( secondHand == 35)  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 40)  {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 45)  {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 50)  {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = NOBLEND;
    }
    if ( secondHand == 55)  {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
    }
  }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
  for ( int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV( random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette() {
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;

  currentPalette = CRGBPalette16(
                     green,  green,  black,  black,
                     purple, purple, black,  black,
                     green,  green,  black,  black,
                     purple, purple, black,  black );
}
