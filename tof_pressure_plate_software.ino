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
#define UPDATES_PER_SECOND 100
#define PLATE_THRESHOLD 500
#define POWER_UP_DELAY_MILLIS 3000
#define DATA_RATE_BAUD        9600
#define COLOR_SELECTOR_MAX_CHARGE 7

CRGB leds[NUM_LEDS];
int intensity = 255;
unsigned int distance = 0;

int plate0 = 0;
int plate1 = 0;
int plate2 = 0;

int rgb[3];
int color_selector = 0;
int color_selector_charge = 0;

char* current_color(int color_selector) {
  if (color_selector == 0) {
    return "red";
  } else if (color_selector == 1) {
    return "green";
  } else if (color_selector == 2) {
    return "blue";
  }
  return "invalid color";
}

bool is_down(int plate_id) {
  int value = analogRead(plate_id);
  return value < PLATE_THRESHOLD;
}

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

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}


void loop() {
  Serial.print("Charge: ");
  Serial.print(color_selector_charge);
  Serial.print(" Current color is ");
  Serial.println(current_color(color_selector));

  distance_sensor.takeMeasurement();
  if (is_down(A0)) {
    // Active charge-up
    if (color_selector_charge < COLOR_SELECTOR_MAX_CHARGE) {
      color_selector_charge = color_selector_charge + 3;
    } else {
      // At this threshold, switch the color and reset the charge meter.
      color_selector = (color_selector + 1) % 3;
      Serial.print("Changed color to ");
      Serial.println(current_color(color_selector));
      color_selector_charge = 0;
    }
  } else {
    // Passive cooldown
    if (color_selector_charge > 0) {
      color_selector_charge--;
    }
  }

  if (is_down(A1)) {
    Serial.println("Decreasing color.");
    rgb[color_selector] = constrain(rgb[color_selector] - 1, 0, 255);
  }
  if (is_down(A2)) {
    Serial.println("Increasing color.");
    rgb[color_selector] = constrain(rgb[color_selector] + 1, 0, 255);
  }

  FillLeds();
  unsigned int distance = distance_sensor.getDistance(); //Retrieve the distance value
  uint8_t brightness = map(distance, 75, 2045, 0, 255); //may need to adjust parameters 2 and 3 to find appropriate range
  FastLED.setBrightness(brightness);
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLeds() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(rgb[0], rgb[1], rgb[2]);
  }
}
