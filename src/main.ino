#include <Arduino.h>
#include <FastLED.h>
#include <pixeltypes.h>
#include <OneButton.h>

#define POTENTIOMETER_PIN A0
#define RGB_LEDS_PIN 6
#define BUTTON_PIN 12
#define ONBOARD_LED 13

#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    57
#define NUM_MODES 7

int ledMode = 0;
int lastButtonValue = 1;
int indexer = 0;
int lastLevel = 0;
uint8_t brightness = 200;
uint8_t lastBrightness = 200;
uint8_t paletteIndex = 0;
uint8_t hue = 0;
boolean cyclingMode = false;
boolean color1 = true;
CRGB maroon = CRGB(200, 0, 0);
CRGB gold = CRGB(223, 188, 0);
CRGB leds[NUM_LEDS];

DEFINE_GRADIENT_PALETTE (maroonAndGold) {
  0,   128,   0,   0,   //maroon
  50,   128,   0,   0,   //maroon
  127,   223,   188,   0,   //gold
  205,   128,   0,   0,   //maroon
  255, 128,   0,   0,   //maroon
};
CRGBPalette16 palette = maroonAndGold;

OneButton btn = OneButton(BUTTON_PIN, true, true);

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);   // Define pin #12 as input and activate the internal pull-up resistor
  pinMode(ONBOARD_LED, OUTPUT);  // Define pin #13 as output, for the LED

  FastLED.addLeds<CHIPSET, RGB_LEDS_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( brightness );
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, 500);

  btn.attachClick(buttonClick);
  btn.attachLongPressStart(enterCyclingMode);
}

void loop()
{
  btn.tick(); // OneButton required statement for reading button value

  int buttonValue = digitalRead(BUTTON_PIN);  // Basic code for activating led on the arduino with button press for debugging
  if (buttonValue == LOW){
    digitalWrite(ONBOARD_LED, HIGH);
  } else {
    digitalWrite(ONBOARD_LED, LOW);
  }

  uint16_t potRead = analogRead(POTENTIOMETER_PIN);
  uint8_t recBrightness = map(potRead, 0, 1023, 0, 255);
  if (recBrightness <= 1) {
    brightness = 0;
  } else if (abs(recBrightness - lastBrightness) > 1) {
    brightness = recBrightness;
    lastBrightness = brightness;
  }

  FastLED.setBrightness( brightness );

  if (cyclingMode) {
    EVERY_N_SECONDS(10){
      ledMode = (ledMode + 1) % 4;
    }
  }

  switch(ledMode) {
    case 0:
      maroonAndGoldCycle();
      break;
    case 1:
      goldSparkleOnMaroon();
      break;
    case 2:
      fadeToBlackBy(leds, NUM_LEDS, 10);
      cornerLights();
      break;
    case 3:
      maroonAndGoldStreaks();
      break;
    case 4:
      rainbowWave();
      break;
    case 5:
      rainbow();
      break;
    case 6:
      commet();
    default:
      break;
  }
  
  FastLED.show(); // display this frame
}

void buttonClick() {
  cyclingMode = false;
  nextMode();
  if (ledMode == 1) {
    fill_solid(leds, NUM_LEDS, maroon);
  }
}

void enterCyclingMode() {
  cyclingMode = true;
}

void nextMode() {
  ledMode = (ledMode + 1) % NUM_MODES; // Change the number after the % to the number of patterns you have
}

void maroonAndGoldCycle() { // Effect 1
  fill_palette(leds, NUM_LEDS, paletteIndex, 255 / NUM_LEDS * 5, palette, 255, LINEARBLEND);

  EVERY_N_MILLISECONDS(10){
    paletteIndex+= 2;
  }
}

void cornerLights() { // Effect 3
  int level = beatsin16(15, -5, 255, 0, 0);
  if (level < 3) {
    level = 0;
    if (lastLevel >= 3) {
      color1 = color1 != true;
    }
  }
  lastLevel = level;
  int cornerLeds1 [] = {13, 14, 15, 16, 40, 41, 42, 43, 44, 45};
  int cornerLeds2 [] = {1, 2, 3, 26, 27, 28, 29, 30, 54, 55, 56};

  for (int c : cornerLeds1) {
    if (color1) {
      leds[c] = CHSV(0, 255, level * 0.7);
    } else {
      leds[c] = CHSV(36, 255, level * 0.9);
    }
  }
  for (int c : cornerLeds2) {
    if (color1) {
      leds[c] = CHSV(36, 255, level * 0.9);
    } else {
      leds[c] = CHSV(0, 255, level * 0.7);
    }
  }

  blur1d(leds, NUM_LEDS, 150);
}

void maroonAndGoldStreaks() { // Effect 4
  fadeToBlackBy(leds, NUM_LEDS, 3);
  EVERY_N_MILLISECONDS(40) {
    fadeToBlackBy(leds, NUM_LEDS, 20);
    indexer = (indexer + 1) % NUM_LEDS;
    leds[indexer] = maroon;
    leds[NUM_LEDS - indexer] = gold;
  }
}

// Effect 2
// Base code by kriegsman https://gist.github.com/kriegsman/88954aae22b03a664081
#define BASE_COLOR maroon // Base color underneath sparkle
#define PEAK_COLOR gold // Peak color to twinkle up to
#define DELTA_COLOR_UP   CRGB(5,5,0)  // Amount to increment the color by each loop as it gets brighter:
#define DELTA_COLOR_DOWN CRGB(2,2,0)  // Amount to decrement the color by each loop as it gets dimmer:
#define CHANCE_OF_TWINKLE 1 // Chance for pixels to begin brightening. 1 or 2 = a few brightening pixels at a time, 10 = lots of pixels brightening at a time.

enum { SteadyDim, GettingBrighter, GettingDimmerAgain };
uint8_t PixelState[NUM_LEDS];

void goldSparkleOnMaroon() {
  EVERY_N_MILLISECONDS(10) {
    for( uint16_t i = 0; i < NUM_LEDS; i++) {
      if( PixelState[i] == SteadyDim) {
        if( random8() < CHANCE_OF_TWINKLE) { // Random decision to increase brightness
          PixelState[i] = GettingBrighter;
        }
      } else if( PixelState[i] == GettingBrighter ) {
        if( leds[i] >= PEAK_COLOR ) {
          PixelState[i] = GettingDimmerAgain;
        } else {
          leds[i] += DELTA_COLOR_UP;  // otherwise, just keep brightening it:
        }
      } else { // This pixels is currently: GettingDimmerAgain so if it's back to base color, switch it to steady dim
        if( leds[i] <= BASE_COLOR ) {
          leds[i] = BASE_COLOR; // reset to exact base color, in case we overshot
          PixelState[i] = SteadyDim;
        } else {
          leds[i] -= DELTA_COLOR_DOWN; // otherwise, just keep dimming it down:
        }
      }
    }
  }
}

void rainbowWave() {
  EVERY_N_MILLISECONDS (10) {
    hue++;
  }
  fill_rainbow(leds, NUM_LEDS, hue, 10);
}

void rainbow() {

  fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));

  EVERY_N_MILLISECONDS(50) {
    hue++;
  }
}

void commet() {
  EVERY_N_MILLISECONDS(40) {
    const byte fadeAmt = 128;
    const int cometSize = 3;
    const int deltaHue  = 4;

    static byte hue = HUE_RED;
    static int iDirection = 1;
    static int iPos = 0;

    hue += deltaHue;

    iPos += iDirection;
    if (iPos == (NUM_LEDS - cometSize) || iPos == 0)
        iPos = 0;
    
    for (int i = 0; i < cometSize; i++)
        leds[iPos + i].setHue(hue);
    
    // Randomly fade the LEDs
    for (int j = 0; j < NUM_LEDS; j++)
        if (random(10) > 5)
            leds[j] = leds[j].fadeToBlackBy(fadeAmt); 
  }
}