//#include <Adafruit_NeoPixel.h>
//#ifdef __AVR__
// #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
//#endif

#include <Arduino.h>
#include <FastLED.h>
#include <pixeltypes.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
//#define LED_PIN    6
//
//// How many NeoPixels are attached to the Arduino?
//#define LED_COUNT 12

#define POTENTIOMETER_PIN A0

// Declare the pins for the Button and the LED<br>
int buttonPin = 12;
int LED = 13;

#define LED_PIN     6
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    56

#define FRAMES_PER_SECOND 60

int ledMode = 0;
int numModes = 4;
int brightness = 200;
bool gReverseDirection = false;
int lastButtonValue = 1;

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  // Define pin #12 as input and activate the internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);
  // Define pin #13 as output, for the LED
  pinMode(LED, OUTPUT);
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( brightness );
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, 600);
}

void loop()
{
   //Serial.println(analogRead(POTENTIOMETER_PIN));
   brightness = analogRead(POTENTIOMETER_PIN)/1023.0*255;
   brightness = brightness <= 10 ? 0 : brightness;
   Serial.println(brightness);
   // Read the value of the input. It can either be 1 or 0
   int buttonValue = digitalRead(buttonPin);
   Serial.println(buttonValue);
   if (buttonValue == HIGH && lastButtonValue == LOW) {
      ledMode++;
      if (ledMode >= numModes) {
        ledMode = 0;
      }
   }
   lastButtonValue = buttonValue;
   Serial.println(ledMode);

//   if (buttonValue == LOW){
//      // If button pushed, turn LED on
//      digitalWrite(LED,HIGH);
//   } else {
//      // Otherwise, turn the LED off
//      digitalWrite(LED, LOW);
//   }
  // Add entropy to random number generator; we use a lot of it.
  // random16_add_entropy( random());

  FastLED.setBrightness( brightness );

  switch(ledMode) {
    case 0:
      for( int j = 0; j < NUM_LEDS; j++) {
        leds[j] = CRGB::Red;
      }
      break;
    case 1:
      for( int j = 0; j < NUM_LEDS; j++) {
        leds[j] = CRGB::White;
      }
      break;
    case 2:
      Fire2012();
      break;
    case 3:
      IrondaleLEDMode();
      break;
    default:
      break;
  }

//  for( int j = 0; j < NUM_LEDS; j++) {
//    leds[j] = CRGB::Red;
//  }
  //Fire2012(); // run simulation frame
  
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
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

void IrondaleLEDMode() {
  boolean isUp = true;
  for( int j = 0; j < NUM_LEDS; j++) {
    
    
    leds[j] = CRGB(j*3, 255, 255);
  }

  
}



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