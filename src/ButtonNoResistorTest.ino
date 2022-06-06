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
#define FRAMES_PER_SECOND 60

enum potMode {brightnessAdjust, ledmode, framerate};

potMode currentPotMode = brightnessAdjust;
int ledMode = 0;
int numModes = 6;
int brightness = 200;
int lastBrightness = 200;
int lastButtonValue = 1;
uint8_t paletteIndex = 0;
uint8_t hue = 0;
int indexer = 0;
boolean cyclingMode = false;
boolean color1 = true;

CRGB maroon = CRGB(128, 0, 0);
CRGB gold = CRGB(223, 188, 0);

OneButton btn = OneButton(BUTTON_PIN, true, true);

DEFINE_GRADIENT_PALETTE (maroonAndGold) {
  0,   128,   0,   0,   //maroon
  100,   128,   0,   0,   //maroon
  255, 223,   188,   0,   //gold
};

DEFINE_GRADIENT_PALETTE (maroonAndGold2) {
  0,   128,   0,   0,   //maroon
  50,   128,   0,   0,   //maroon
  127,   223,   188,   0,   //maroon
  205,   128,   0,   0,   //maroon
  255, 128,   0,   0,   //gold
};

CRGBPalette16 palette = maroonAndGold2;

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);   // Define pin #12 as input and activate the internal pull-up resistor
  pinMode(ONBOARD_LED, OUTPUT);  // Define pin #13 as output, for the LED
  delay(3000); // sanity delay

  FastLED.addLeds<CHIPSET, RGB_LEDS_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( brightness );
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, 500);

  btn.attachClick(buttonClick);
  btn.attachLongPressStart(enterCyclingMode);
}

void loop()
{
  btn.tick();

  uint16_t potRead = analogRead(POTENTIOMETER_PIN);
  // switch (currentPotMode)
  // {
  // case brightnessAdjust:
    uint8_t recBrightness = map(potRead, 0, 1023, 0, 255);
    if (recBrightness <= 1) {
      brightness = 0;
    } else if (abs(recBrightness - lastBrightness) > 1) {
      brightness = recBrightness;
      lastBrightness = brightness;
    }
  //   break;
  // case ledmode:
  //   int mode = map(potRead, 0, 1023, 0, 20);
  //   ledMode = mode;
  //   Serial.println(mode);
  //   break;
  // case framerate:
  //   break;
  // default:
  //   break;
  // }

  // Serial.println(ledMode);
  FastLED.setBrightness( brightness );

  int buttonValue = digitalRead(BUTTON_PIN);
  if (buttonValue == LOW){
    // If button pushed, turn LED on
    digitalWrite(ONBOARD_LED, HIGH);
  } else {
    // Otherwise, turn the LED off
    digitalWrite(ONBOARD_LED, LOW);
  }

  if (cyclingMode) {
    EVERY_N_SECONDS(10){
      nextMode();
    }
  }

  switch(ledMode) {
    case 0:
      IrondaleLEDMode1();
      break;
    case 1:
      rainbowWave();
      break;
    case 2:
      EVERY_N_MILLISECONDS(40){
        Fire2012();
      }
      break;
    case 3:
      rainbow();
      break;
    case 4:
      //fadeToBlackBy(leds, NUM_LEDS, 30);
      cornerLights();
      break;
    case 5:
      commet();
    default:
      break;
  }
  
  FastLED.show(); // display this frame
  //FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void buttonClick() {
  cyclingMode = false;
  nextMode();
}

void nextMode() {
  ledMode = (ledMode + 1) % numModes; // Change the number after the % to the number of patterns you have
}

// void potLEDMode() {
//   if (currentPotMode == ledmode) {
//     currentPotMode = brightnessAdjust;
//   } else {
//     currentPotMode = ledmode;
//   }
// }

void enterCyclingMode() {
  cyclingMode = true;
}

void white() {
  // int level = beatsin16(20, -20, 255, 0, 0);
  // if (level < 3) {
  //   level = 0;
  // }
  // for( int j = 0; j < NUM_LEDS; j++) {
  //   leds[j] = CHSV(0, 0, level);
  // }
  for( int j = 0; j < NUM_LEDS; j++) {
    leds[j] = CRGB::White;
  }
}

void IrondaleLEDMode1() {
  fill_palette(leds, NUM_LEDS, paletteIndex, 255 / NUM_LEDS * 5, palette, 255, LINEARBLEND);

  EVERY_N_MILLISECONDS(10){
    paletteIndex+= 3;
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

  EVERY_N_MILLISECONDS(20) {
    hue++;
  }
}

void cornerLights() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  int level = beatsin16(20, -20, 255, 0, 0);
  // Serial.println(level);
  // if (level <= -19) {
  //   color1 = color1 ? false : true;
  // }
  if (level < 5) {
    level = 0;
  }
  // int cornerLeds1 [] = {14, 15, 41, 42, 43};
  // int cornerLeds2 [] = {2, 27, 28, 29, 55, 56};
  // for (int c : cornerLeds1) {
  //   if (color1) {
  //     leds[c] = CHSV(0, 255, level * 0.5);
  //   } else {
  //     leds[c] = CHSV(36, 255, level * 0.87);
  //   }
    
  // }
  int cornerLeds [] = {14, 15, 41, 42, 43, 2, 27, 28, 29, 55, 56};
  for (int c : cornerLeds) {
    leds[c] = CHSV(0, 0, level);
  }
  blur1d(leds, NUM_LEDS, 172);
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

void commet() {
  EVERY_N_MILLISECONDS(20) {
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