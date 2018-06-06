# RickandMorty
Integrating Software and Hardware
PortalGun

*****All resources used were originally found at:  

https://www.thingiverse.com/thing:1100601 and from there - 
https://github.com/pomeroyb/PortalGun

I have altered the pin-outs as necessary to allow for more sounds and for better organization of the wiring*****
Code to control a Portal Gun (From Rick and Morty)
##Libraries Download and install the following libraries:
•	ClickEncoder
•	Adafruit_GFX
•	Adafruit_LEDBackpack
Pin Definitions
If you deviate from the following definitions, you will have to change the firmware to account for that.
 
LED Display	Trinket Pro Pin
SCL	A5
SDA	A4
GND	GND
Vcc	5V
Vi2c	5V
Rotary Encoder	Trinket Pro Pin
A	A1
B	A0
GND	GND
Button	A2
LED	Trinket Pro Pin
Top Bulb	13
Front Right	12
Front Center	11
Front Left	10
 
Installing Firmware
First, set up the Arduino IDE according to Adafruit. Connect your Trinket Pro and make sure the bootloader is running, then click upload.
Button Behavior
The rotary encoder has a click button, and we can detect a single click, a double click, and a hold.
•	Single Click : Wakes the Trinket Pro from low power mode
•	Double Click : Reset to dimension C137
•	Hold : Turn off LEDs and put the Trinket Pro into a low power mode.
##Installing SFX The main branch doesn't have SFX support. Use the SFX branch to test this out (I found that the speaker inside the case was too quiet to hear.)

PortalGun
Code to control a Portal Gun (From Rick and Morty)
##Libraries Download and install the following libraries:
•	ClickEncoder
•	Adafruit_GFX
•	Adafruit_LEDBackpack
Pin Definitions
If you deviate from the following definitions, you will have to change the firmware to account for that.
 


 
FX SoundBoard	Trinket Pro Pin	Sound Effect Dimension Location
For these locations you can use any sound file you want, I used VLC media files (.OGG) per the specifications by Adafruit
0	0	C136 – this is the value entered on the LED display
1	1	C135
2	2	C134
3	3	C133
4	4	C132
5	5	C131
6	6	C130 
7	7	C129
8	8	C128
9	9	C127
 
 
Installing Firmware
First, set up the Arduino IDE according to Adafruit. Connect your Trinket Pro and make sure the bootloader is running, then click upload.
