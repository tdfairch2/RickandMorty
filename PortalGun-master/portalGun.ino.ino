#include <ClickEncoder.h>

#include <TimerOne.h>

// Code to control a Rick and Morty Portal Gun
// Written by Brandon Pomeroy, 2015

/* Modified by Terry Fairchild, 2018
  all 3-d print files and code source files -
  https://www.thingiverse.com/thing:1100601
      and -
  "People who are really serious about software should build their own hardware" - Alan Kay*/

/*
******** Required Libraries *************
  ClickEncoder - https://github.com/0xPIT/encoder
  Adafruit_GFX - https://github.com/adafruit/Adafruit-GFX-Library
  Adafruit_LEDBackpack - https://github.com/adafruit/Adafruit-LED-Backpack-Library
*/


/*
********** Required Hardware *********************
  Adafruit Pro Trinket 5V 16MHz - http://www.adafruit.com/product/2000
  LiPoly BackPack - http://www.adafruit.com/product/2124
  LiPoly Battety 3.7V - http://www.adafruit.com/products/1578
  Rotary Encoder - http://www.adafruit.com/products/377
  Metal Knob - http://www.adafruit.com/products/2056
  Quad Alphanumeric Display (Red 0.54") - http://www.adafruit.com/products/1911
  10mm Diffused Green LED (x4) - https://www.adafruit.com/products/844
  10mm Plastic Bevel LED Holder (x4) - https://www.adafruit.com/products/2171
  150 Ohm Resistor (x4) for LEDs
  Inductive Charging Set - 5V - https://www.adafruit.com/products/1407
  2.1mm Panel Mount Barrel Jack - http://www.adafruit.com/products/610
  9VDC Power Supply - http://www.adafruit.com/products/63
*/

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <avr/sleep.h>
#include <avr/power.h>

// Set up our LED display
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
char displayBuffer[4];
uint8_t dimensionLetter = 'C';

// Set up the click encoder
ClickEncoder *encoder;
int16_t last, value;
#define encoderPinA          A1
#define encoderPinB          A0
#define encoderButtonPin     A2

// Steps per notch can be 1, 4, or 8. If your encoder is counting
// to fast or too slow, change this!
#define stepsPerNotch        4

// Comment this line to make the encoder increment in the opposite direction
#define reverseEncoderWheel

// FX Board output delay (ms)
const int msDelay = 500;


/***********************************************************
*       Set up the sound fx pinouts
* T03 doesn't work & I can't resolve the issue
* T06 & T07 are probably offensive 
* and have been placed at C106 and C107
************************************************************/
#define portalGunPin         0      //Portal, T00
#define sound01              1      //Pickle Rick, T01
#define sound02              2      //Tiny Rick, T02
#define sound03              3      //Mr Meeseeks, T03  This one doesn't work
#define sound04              4      //Peace oot, T04
#define sound05              5      //Wrecked, T05
#define sound06              6      //Party bitch, T06 
#define sound07              7      //Lick my balls T07
#define sound08              8      //Wrecked, T05
#define sound09              9      //Party bitch, T06
/************************************************************/
/************************************************************/


// Set up the Green LEDs
#define topBulbPin           13
#define frontRightPin        12
#define frontCenterPin       11
#define frontLeftPin         10
#define maximumBright        255
#define mediumBright         127
int topBulbBrightness = 255;

// Set up what we need to sleep/wake the Trinket
// Define the pins you'll use for interrupts - CHANGE THESE to match the input pins
// you are using in your project
#define NAV0_PIN A2
#define NAV1_PIN 1
#define NAV2_PIN 2


//Let us know if our Trinket woke up from sleep
volatile bool justWokeUp;


void timerIsr() {
  encoder->service();
}


void setup() {
  enablePinInterupt(NAV0_PIN);
  enablePinInterupt(NAV1_PIN);
  enablePinInterupt(NAV2_PIN);

  //Set up pin modes
  pinMode(topBulbPin, OUTPUT);
  pinMode(frontRightPin, OUTPUT);
  pinMode(frontLeftPin, OUTPUT);
  pinMode(frontCenterPin, OUTPUT);


/*************************************************************************/
/****Initialize the sound fx pinouts and topBulb to be output pins *******/
/*************************************************************************/
  pinMode(portalGunPin, OUTPUT);
  pinMode(sound01, OUTPUT);
  pinMode(sound02, OUTPUT);
  pinMode(sound03, OUTPUT);
  pinMode(sound04, OUTPUT);
  pinMode(sound05, OUTPUT);
  pinMode(sound06, OUTPUT);
  pinMode(sound07, OUTPUT);
  pinMode(sound08, OUTPUT);
  pinMode(sound09, OUTPUT);
/*************************************************************************/
/*************************************************************************/


  digitalWrite(frontRightPin, LOW);
  digitalWrite(frontLeftPin, LOW);
  digitalWrite(frontCenterPin, LOW);
  digitalWrite(topBulbPin, HIGH); //topBulbPin is for the plasma chamber and is illuminated until shutdown


/********************************************************************************
 * Initialize the sound fx pins to be OFF
 * Counter-intuitively when these pins are set to 'LOW' 
 * they become active, then become inactive when set to 'HIGH'.
 *******************************************************************************/
  digitalWrite(portalGunPin, HIGH);
  digitalWrite(sound01, HIGH);
  digitalWrite(sound02, HIGH);
  digitalWrite(sound03, HIGH);
  digitalWrite(sound04, HIGH);
  digitalWrite(sound05, HIGH);
  digitalWrite(sound06, HIGH);
  digitalWrite(sound07, HIGH);
  digitalWrite(sound08, HIGH);
  digitalWrite(sound09, HIGH);
/******************************************************************************
 ******************************************************************************/


  encoderSetup();
  alpha4.begin(0x70);  // pass in the address for the LED display

  justWokeUp = false;

}


void loop() {
  if (justWokeUp) {


/***************************************************************************/
/*****  At 'wake-up', sound fx are turned off by setting to HIGH  **********
******* Seems redundant but otherwise they trigger constantly  ************/
    digitalWrite(portalGunPin, HIGH);    
    digitalWrite(sound01, HIGH);
    digitalWrite(sound02, HIGH);
    digitalWrite(sound03, HIGH);
    digitalWrite(sound04, HIGH);
    digitalWrite(sound05, HIGH);
    digitalWrite(sound06, HIGH);
    digitalWrite(sound07, HIGH);
    digitalWrite(sound08, HIGH);
    digitalWrite(sound09, HIGH);
/***************************************************************************/
/***************************************************************************/


    digitalWrite(frontRightPin, LOW);
    digitalWrite(frontLeftPin, LOW);
    digitalWrite(frontCenterPin, LOW);
    digitalWrite(topBulbPin, HIGH);
    justWokeUp = false;
  }

  ClickEncoder::Button b = encoder->getButton();
  switch (b) {
    case ClickEncoder::Held:
      // Holding the button will put your trinket to sleep.
      // The trinket will wake on the next button press
      alpha4.clear();
      alpha4.writeDigitAscii(0, 'R');
      alpha4.writeDigitAscii(1, 'I');
      alpha4.writeDigitAscii(2, 'C');
      alpha4.writeDigitAscii(3, 'K');
      digitalWrite(frontRightPin, LOW);
      digitalWrite(frontLeftPin, LOW);
      digitalWrite(frontCenterPin, LOW);
      digitalWrite(topBulbPin, LOW);


/***************************************************************************
*  At shut-down, sound fx are turned off
****************************************************************************/
      digitalWrite(portalGunPin, HIGH);
      digitalWrite(sound01, HIGH);
      digitalWrite(sound02, HIGH);
      digitalWrite(sound03, HIGH);
      digitalWrite(sound04, HIGH);
      digitalWrite(sound05, HIGH);
      digitalWrite(sound06, HIGH);
      digitalWrite(sound07, HIGH);
      digitalWrite(sound08, HIGH);
      digitalWrite(sound09, HIGH);
/***************************************************************************
***************************************************************************/


      alpha4.writeDisplay();
      delay(2000);
      alpha4.clear();
      alpha4.writeDisplay();


/**************************************************************************
*           Original code block above displayed "RICK" only
*  The following modification scrolls "RICK and MORTY" during shutdown
/**************************************************************************/
      alpha4.clear();
      alpha4.writeDigitAscii(0, 'a');
      alpha4.writeDigitAscii(1, 'n');
      alpha4.writeDigitAscii(2, 'd');
      alpha4.writeDigitAscii(3, ' ');
      alpha4.writeDisplay();
      delay(750);
      alpha4.clear();
      alpha4.writeDisplay();
      delay(500);

      alpha4.clear();
      alpha4.writeDigitAscii(0, 'M');
      alpha4.writeDigitAscii(1, 'O');
      alpha4.writeDigitAscii(2, 'R');
      alpha4.writeDigitAscii(3, 'T');
      alpha4.writeDisplay();
      delay(500);

      alpha4.clear();
      alpha4.writeDigitAscii(0, 'O');
      alpha4.writeDigitAscii(1, 'R');
      alpha4.writeDigitAscii(2, 'T');
      alpha4.writeDigitAscii(3, 'Y');
      alpha4.writeDisplay();
      delay(500);
      
      alpha4.clear();
      alpha4.writeDigitAscii(0, 'R');
      alpha4.writeDigitAscii(1, 'T');
      alpha4.writeDigitAscii(2, 'Y');
      alpha4.writeDigitAscii(3, ' ');
      alpha4.writeDisplay();
      delay(500);

      alpha4.clear();
      alpha4.writeDigitAscii(0, 'T');
      alpha4.writeDigitAscii(1, 'Y');
      alpha4.writeDigitAscii(2, ' ');
      alpha4.writeDigitAscii(3, ' ');
      alpha4.writeDisplay();
      delay(500);


      alpha4.clear();
      alpha4.writeDigitAscii(0, 'Y');
      alpha4.writeDigitAscii(1, ' ');
      alpha4.writeDigitAscii(2, ' ');
      alpha4.writeDigitAscii(3, ' ');
      alpha4.writeDisplay();
      delay(500);
      alpha4.clear();
      alpha4.writeDisplay();
      delay(1000);
/***********************************************************************************
************************************************************************************/


      justWokeUp = true;
      goToSleep();
      break;

    case ClickEncoder::Clicked:
      // When the encoder wheel is single clicked


/***************************************************************************************
* Check if this is a sound fx dimension
* Sound will trigger at specific locations when entered AND button is pressed once 
* File names are listed in the comments above when the sound files are defined 
* When triggered the output requires a delay of at least 200ms
***************************************************************************************/
      if (dimensionLetter == 'C' && value == 136){
                  delay (500);
                  digitalWrite(sound01, LOW);
                  delay (500);
                  digitalWrite(sound01, HIGH);
      }

      else if (dimensionLetter == 'C' && value == 135){
                  delay (500);
                  digitalWrite(sound02, LOW);
                  delay (500);
                  digitalWrite(sound02, HIGH);
      }
      else if (dimensionLetter == 'C' && value == 134){
                  delay (500);
                  digitalWrite(sound04, LOW);
                  delay (500);
                  digitalWrite(sound04, HIGH);
      }
      else if (dimensionLetter == 'C' && value == 133){
                  delay (500);
                  digitalWrite(sound05, LOW);
                  delay (500);
                  digitalWrite(sound05, HIGH);
      }
      else if (dimensionLetter == 'C' && value == 106){
                  delay (500);
                  digitalWrite(sound06, LOW);
                  delay (500);
                  digitalWrite(sound06, HIGH);
      }
      else if (dimensionLetter == 'C' && value == 107){
                  delay (500);
                  digitalWrite(sound07, LOW);
                  delay (500);
                  digitalWrite(sound07, HIGH);            
      }
      else if (dimensionLetter == 'C' && value == 132){
                  delay (500);
                  digitalWrite(sound08, LOW);
                  delay (500);
                  digitalWrite(sound08, HIGH);
      }
      else if (dimensionLetter == 'C' && value == 131){
                  delay (500);
                  digitalWrite(sound09, LOW);
                  delay (500);
                  digitalWrite(sound09, HIGH);            
            }
/************************************************************************************
* If a sound fx location is not selected, run the Firing Sequence.
* Flash the LED's and the word FIRE five times simultaneously
*  then four times flash the LED's only  
************************************************************************************/
        else {                                              //
        digitalWrite(portalGunPin, LOW);                    //
        delay(500);                                         //
        digitalWrite(portalGunPin, HIGH);                   //
                                                            //
      for (int i = 1; i <= 5; i++) {                        //
        alpha4.clear();                                     //
        alpha4.writeDigitAscii(0, 'F');                     //
        alpha4.writeDigitAscii(1, 'I');                     //
        alpha4.writeDigitAscii(2, 'R');                     //
        alpha4.writeDigitAscii(3, 'E');                     //
        digitalWrite(frontRightPin, LOW);                   //
        digitalWrite(frontLeftPin, LOW);                    //
        digitalWrite(frontCenterPin, LOW);                  //
                                                            //
        alpha4.writeDisplay();                              //
        delay(250);                                         //
        alpha4.clear();                                     //
        digitalWrite(frontRightPin, HIGH);                  //
        digitalWrite(frontLeftPin, HIGH);                   //
        digitalWrite(frontCenterPin, HIGH);                 //
                                                            //
        alpha4.writeDisplay();                              //
        delay(250);                                         //
      }                                                     //
      for (int i = 1; i <= 4; i++) {                        //
        alpha4.clear();                                     //
        digitalWrite(frontRightPin, LOW);                   //
        digitalWrite(frontLeftPin, LOW);                    //
        digitalWrite(frontCenterPin, LOW);                  //
                                                            //
        alpha4.writeDisplay();                              //
        delay(250);                                         //
        alpha4.clear();                                     //
        digitalWrite(frontRightPin, HIGH);                  //
        digitalWrite(frontLeftPin, HIGH);                   //
        digitalWrite(frontCenterPin, HIGH);                 //
                                                            //
        alpha4.writeDisplay();                              //
        delay(250);                                         //
        digitalWrite(frontRightPin, LOW);                   //
        digitalWrite(frontLeftPin, LOW);                    //
        digitalWrite(frontCenterPin, LOW);                  //
                                                            //
        }                                                   //
      }                                                     //
/***************************************************************************
****************************************************************************/


      break;
    case ClickEncoder::DoubleClicked:
      //If you double click the button, it sets the dimension to C137
      dimensionLetter = 'C';
      value = 137;
      break;

    case ClickEncoder::Open:
      // The dimension will increment from 0-999, then roll over to the next
      // letter. (A999 -> B000)
      updateDimension();
      break;
  }
}


void encoderSetup() {
  // set up encoder
  encoder = new ClickEncoder(encoderPinA, encoderPinB, encoderButtonPin, stepsPerNotch);
  encoder->setAccelerationEnabled(true);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  last = -1;
  value = 137;
}


void updateDimension() {
#ifdef reverseEncoderWheel
  value -= encoder->getValue();
#endif

#ifndef reverseEncoderWheel
  value += encoder->getValue();
#endif

  if (value != last) {
    if (value > 999) {
      value = 0;
      if (dimensionLetter == 'Z') {
        dimensionLetter = 'A';
      } else {
        dimensionLetter ++;
      }
    } else if ( value < 0 ) {
      value = 999;
      if (dimensionLetter == 'A') {
        dimensionLetter = 'Z';
      } else {
        dimensionLetter --;
      }
    }
    last = value;
  }

  sprintf(displayBuffer, "%03i", value);
  alpha4.clear();
  alpha4.writeDigitAscii(0, dimensionLetter);
  alpha4.writeDigitAscii(1, displayBuffer[0]);
  alpha4.writeDigitAscii(2, displayBuffer[1]);
  alpha4.writeDigitAscii(3, displayBuffer[2]);

  alpha4.writeDisplay();
}

/*
  ============== Sleep/Wake Methods ==================
  ====================================================
*/

// Most of this code comes from seanahrens on the adafruit forums
// http://forums.adafruit.com/viewtopic.php?f=25&t=59392#p329418


void enablePinInterupt(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

void goToSleep()
{
  // The ATmega328 has five different sleep states.
  // See the ATmega 328 datasheet for more information.
  // SLEEP_MODE_IDLE -the least power savings
  // SLEEP_MODE_ADC
  // SLEEP_MODE_PWR_SAVE
  // SLEEP_MODE_STANDBY
  // SLEEP_MODE_PWR_DOWN -the most power savings
  // I am using the deepest sleep mode from which a
  // watchdog timer interrupt can wake the ATMega328




  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode.
  sleep_enable(); // Enable sleep mode.
  sleep_mode(); // Enter sleep mode.
  // After waking the code continues
  // to execute from this point.

  sleep_disable(); // Disable sleep mode after waking.
}

ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
{
  // if I wired up D8-D13 then I'd need some code here
}

ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here // NAV0
{
  /* This will bring us back from sleep. */

  /* We detach the interrupt to stop it from
     continuously firing while the interrupt pin
     is low.
  */

  detachInterrupt(0);

}

ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here // NAV1, NAV2
{
  // Check it was NAV1 or NAV2 and nothing else
}


/*
  ============== Testing Methods ==================
  =================================================
uncomment this to make the display run through a test at startup
It may prove useful, it was included in the original code from Brandon Pomeroy


displayTest();


void displayTest() {
  alpha4.writeDigitRaw(3, 0x0);
  alpha4.writeDigitRaw(0, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(0, 0x0);
  alpha4.writeDigitRaw(1, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(1, 0x0);
  alpha4.writeDigitRaw(2, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(2, 0x0);
  alpha4.writeDigitRaw(3, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);

  alpha4.clear();
  alpha4.writeDisplay();

  // display every character,
  for (uint8_t i = '!'; i <= 'z'; i++) {
    alpha4.writeDigitAscii(0, i);
    alpha4.writeDigitAscii(1, i + 1);
    alpha4.writeDigitAscii(2, i + 2);
    alpha4.writeDigitAscii(3, i + 3);
    alpha4.writeDisplay();
    delay(300);
  }
}*/
