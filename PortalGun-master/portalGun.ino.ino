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


/*************************************************************************************************************************************************************************
*                       Set up the sound fx pinouts
* The soundboard allows for only 10 ogg files because it has 10 output pins and the files stored in its memory are identified by number,
*  starting with 01 and ending with 10, depending on how many sound files are to be used.
* 
* 
* For the Pro Trinket, the sound file names are merely references.  I think they can be anything.  They are defined in the statements below.  
*    These names trigger the output pins to which they are assigned, listed next to them.
*
* The six Pro Trinket output pins available for sounds are pins 3 thru 9. Pin 8 is not labeled on the Pro Trinket so I skipped it.  For clarity each of the six pins used 
*    is connected to the corresponding numbered pin on the sound board.
*
* The numbered pin on the sound board plays whichever ogg file has that number.  i.e. sound file 'T04' is the name of an ogg audio file on the sound board.
*    Sound file T04 will be output whenever pin 04 on the sound board is triggered.  Pin 04 is connected to Trinket pin 4, and it is assigned to the variable name 'pickleRick'
*    in the sketch.  So when the sketch correctly uses the name 'pickleRick', it triggers pin 4 on the Pro Trinket which triggers pin 04 on the sound board, 
*    which causes ogg file T04 to play.
*
* Sound files T00, T01, T02, T08 and T10 are included on soundboard.  They have not been integrated into the code because I think there are fewer pinouts on the pro trinket
*    available for sound than were available on the Arduino board I used to prototype this project, and I did not wish to go to the effort to test this.
*
* Just make sure that 1) the pinouts on the Pro Trinket connect to the desired pinouts on the sound board and 2) the names of the sound files match the desired pinouts on the
*    sound board.  If you get either of these wrong you will not get the sound file you want for your dimension location
/*************************************************************************************************************************************************************************/
#define portalGunPin         3      //Portal, T03
#define pickleRick           4      //Pickle Rick, T04
#define tinyRick             5      //Tiny Rick, T05
#define peaceOot             6      //Peace oot, T06 
#define partyBiiiiitch       7      //Party bitch, T07
#define riggityWreckedSon    9      //Wrecked, T09
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
  pinMode(pickleRick, OUTPUT);
  pinMode(tinyRick, OUTPUT);
  pinMode(peaceOot, OUTPUT);
  pinMode(partyBiiiiitch, OUTPUT);
  pinMode(riggityWreckedSon, OUTPUT);
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
  digitalWrite(pickleRick, HIGH);
  digitalWrite(tinyRick, HIGH);
  digitalWrite(peaceOot, HIGH);
  digitalWrite(partyBiiiiitch, HIGH);
  digitalWrite(riggityWreckedSon, HIGH);
/******************************************************************************
 ******************************************************************************/


  encoderSetup();
  alpha4.begin(0x70);  // pass in the address for the LED display

  justWokeUp = false;

}


void loop() {
  if (justWokeUp) {


/***************************************************************************
*  At 'wake-up', sound fx are turned off by setting to HIGH
* Seems redundant but otherwise they trigger constantly
***************************************************************************/
    digitalWrite(portalGunPin, HIGH);
    digitalWrite(pickleRick, HIGH);
    digitalWrite(tinyRick, HIGH);
    digitalWrite(peaceOot, HIGH);
    digitalWrite(partyBiiiiitch, HIGH);
    digitalWrite(riggityWreckedSon, HIGH);
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
      digitalWrite(pickleRick, HIGH);
      digitalWrite(tinyRick, HIGH);
      digitalWrite(peaceOot, HIGH);
      digitalWrite(partyBiiiiitch, HIGH);
      digitalWrite(riggityWreckedSon, HIGH);
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
                  digitalWrite(peaceOot, LOW);
                  delay (500);
                  digitalWrite(peaceOot, HIGH);
      }

      else if (dimensionLetter == 'C' && value == 135){
                  delay (500);
                  digitalWrite(tinyRick, LOW);
                  delay (500);
                  digitalWrite(tinyRick, HIGH);
      }
      else if (dimensionLetter == 'C' && value == 134){
                  delay (500);
                  digitalWrite(pickleRick, LOW);
                  delay (500);
                  digitalWrite(pickleRick, HIGH);
      }
      else if (dimensionLetter == 'C' && value == 133){
                  delay (500);
                  digitalWrite(partyBiiiiitch, LOW);
                  delay (500);
                  digitalWrite(partyBiiiiitch, HIGH);
      }
      else if (dimensionLetter == 'C' && value == 132){
                  delay (500);
                  digitalWrite(riggityWreckedSon, LOW);
                  delay (500);
                  digitalWrite(riggityWreckedSon, HIGH);
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


/* Some quotes:

“Nobody exists on purpose, nobody belongs anywhere, everybody's gonna die.  Come watch TV.”
          -	Morty Smith, "Rixty Minutes"


“It’s a never-ending fart joke wrapped around a studied look into nihilism.”
          -	Kayla Cobb, ‘Rick and Morty’ Is Better When It Embraces Its Strong Women’, 
          Decider, Oct. 4, 2017


Rick: Great, now I have to take over a whole Planet because of your Stupid Boobs!

Rick: [knife at his throat] Are you a simulation Morty? Are you, you little bitch? Are you a simulation Morty?
Morty: No!
Rick: Ah right. Sorry Morty. You're a good kid. You're a good kid Morty.
[Falls asleep]
Morty: What? Oh God! What a life...

Chief: Well done, Babylegs, you've learned a valuable lesson about teamwork and accepting your own limitations. Now get the fuck out!

Mr Poopy Butthole: Oooh weee! Is this how I die?

Plutonian: Is everyone in your Family an Idiot?
Morty: For sure me and my Dad are!
 


Dr. Wong:  Why didn't you want to come here?

Rick:     Because I don't respect Therapy.  Because I'm a Scientist. Because I invent, transform, create and destroy for a living 
and when I don't like something about the World I change it.  And I don't think going to some rented office in a strip mall 
to listen to an Agent of Averageness explain which words mean which feelings has ever helped anyone do anything.  
I expect it's helped a lot of people get comfortable and stop panicking, which is a state of mind we value in the animals we eat 
but not something I want for myself.  I'm not a cow!  I'm a Pickle!  When I feel like it.  
So, you asked…
Dr. Wong:  Rick, the only connection between your unquestionable intelligence and the sickness destroying your family is that 
everyone in your family, you included, use intelligence to justify sickness.  You seem to alternate between viewing your own mind 
as an unstoppable force and as an inescapable curse.  And I think it's because the only truly unapproachable concept for you is that 
it's your mind within your control.  You chose to come here, you chose to talk to belittle my vocation, just as you chose to become a pickle.  
You are the master of your universe, and yet you are dripping with rat blood and feces.  Your enormous mind literally vegetating 
by your own hand. I have no doubt that you would be bored senseless by therapy, the same way I'm bored when I brush my teeth and wipe my ass.  
Because the thing about repairing, maintaining, and cleaning is it's not an adventure.  There's no way to do it so wrong you might die.  
It's just work.  And the bottom line is, some people are okay going to work, and some people... well, some people would rather die.  
Each of us gets to choose.
*/