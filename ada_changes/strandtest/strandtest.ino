#include "Adafruit_WS2801.h"
#include "Animation_Library.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

/*****************************************************************************
Example sketch for driving Adafruit WS2801 pixels!


  Designed specifically to work with the Adafruit RGB Pixels!
  12mm Bullet shape ----> https://www.adafruit.com/products/322
  12mm Flat shape   ----> https://www.adafruit.com/products/738
  36mm Square shape ----> https://www.adafruit.com/products/683

  These pixels use SPI to transmit the color data, and have built in
  high speed PWM drivers for 24 bit color per pixel
  2 pins are required to interface

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

*****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
uint8_t buttonPin = 2;   // Button for grabbing inputs
uint8_t dataPin  = 4;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 5;    // Green wire on Adafruit Pixels
uint8_t potPin = 0;      // Analog pin for using pot

bool setColor = false;
bool recieveIndex = true;
bool mainInterruptAnimation = false;
int mainCurrentAnimationIndex = 0;
int mainMaxNumAnimations = 5;
int currentColorIndex = 0;
int globalSectorIndex = 0;
int globalDelay = 50;
int colorArray[3] = {128, 128, 0};
byte orangeAttempt[3] = {255,40,0};
byte blueAttempt[3] = {0,190,255};
int allColorsEnabled[3] = {1,1,1};
int onlyBlueEnabled[3] = {0,0,1};
int onlyGreenEnabled[3] = {0,1,0};
int onlyRedEnabled[3] = {1,0,0};
struct Colors {
  int red;
  int green;
  int blue;
  char colorFields[3][10] = {"red", "green", "blue"};
} colors;


// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(16, dataPin, clockPin);
Animation_Library animation = Animation_Library();

// Optional: leave off pin numbers to use hardware SPI
// (pinout is then specific to each board and can't be changed)
//Adafruit_WS2801 strip = Adafruit_WS2801(25);

// For 36mm LED pixels: these pixels internally represent color in a
// different format.  Either of the above constructors can accept an
// optional extra parameter: WS2801_RGB is 'conventional' RGB order
// WS2801_GRB is the GRB order required by the 36mm pixels.  Other
// than this parameter, your code does not need to do anything different;
// the library will handle the format change.  Examples:
//Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin, WS2801_GRB);
//Adafruit_WS2801 strip = Adafruit_WS2801(25, WS2801_GRB);

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), setReadAnalogAndChangeCurrentColor, RISING);

  colors.red = 0;
  colors.green = 0;
  colors.blue = 0;

  animation.setLightStrip(strip);
  strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
}

void setReadAnalogAndChangeCurrentColor(){
  if(!setColor){
    setColor = !setColor;
  }
  mainInterruptAnimation = true;
  animation.setInterruptAnimation();
}

void serialEvent(){
  int recievedData = Serial.parseInt();
  Serial.print("Setting global delay to...");
  Serial.println(recievedData);
  globalDelay = recievedData;
}

void selectCurrentAnimation() {
  switch (mainCurrentAnimationIndex){
    case 0: animation.granularSectorFadeCycle(globalDelay, onlyBlueEnabled, false);
      break;
    case 1: animation.rainbow(globalDelay);
      break;
    case 2: animation.rainbowCycle(globalDelay);
      break;
    case 3: animation.linearSectorCycleAllColors(globalDelay);
      break;
    case 4: animation.randomSetPixel(globalDelay, false);
      break;
    default: animation.granularSectorFadeCycle(globalDelay, onlyBlueEnabled, false);
      break;
  }
}

void loop() {
  // Some example procedures showing how to display to the pixels

//  colorWipe(Color(255, 0, 0), 50);
//  colorWipe(Color(0, 255, 0), 50);
//  colorWipe(Color(0, 0, 255), 50);
//  rainbow(20);

  if(setColor){
    for(int i=0; i<3; i++){
      Serial.print(colorArray[i]);
      if(i!=2){
        Serial.print(",");
      }
    }
    setColor = !setColor;
  }

  // animation.granularSectorFadeCycle(globalDelay, 0);
  selectCurrentAnimation();
  if(mainInterruptAnimation) {
    animation.animationReset(globalDelay);
    mainCurrentAnimationIndex = (mainCurrentAnimationIndex + 1) % mainMaxNumAnimations;
    Serial.print("mainCurrentAnimationIndex:");
    Serial.println(mainCurrentAnimationIndex);
    mainInterruptAnimation = false;
  }
}
