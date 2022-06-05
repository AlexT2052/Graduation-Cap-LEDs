//#include <Adafruit_NeoPixel.h>
//#ifdef __AVR__
// #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
//#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
//#define LED_PIN    6
//
//// How many NeoPixels are attached to the Arduino?
//#define LED_COUNT 12

#include <Arduino.h>

#define POTENTIOMETER_PIN A0

// Declare the pins for the Button and the LED<br>
int buttonPin = 12;
int LED = 13;

void setup() {
   Serial.begin(9600);
   // Define pin #12 as input and activate the internal pull-up resistor
   pinMode(buttonPin, INPUT_PULLUP);
   // Define pin #13 as output, for the LED
   pinMode(LED, OUTPUT);
}

void loop(){
   Serial.println(analogRead(POTENTIOMETER_PIN));
   // Read the value of the input. It can either be 1 or 0
   int buttonValue = digitalRead(buttonPin);
   Serial.println(buttonValue);
   if (buttonValue == LOW){
      // If button pushed, turn LED on
      digitalWrite(LED,HIGH);
   } else {
      // Otherwise, turn the LED off
      digitalWrite(LED, LOW);
   }
   delay(100);
}
