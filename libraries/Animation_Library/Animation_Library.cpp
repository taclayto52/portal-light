#include "Animation_Library.h"
#include "Adafruit_WS2801.h"

#define NUM_SECTORS 8
#define NUM_PIXELS_PER_SECTOR 2

int sectorIndices[8][2] = {{5,13},{6,14},{7,15}, {0,8}, {1,9}, {2,10}, {3,11}, {4,12}};
int sectorFadeValues[8] = {255,223,191,159,127,95,63,31};
int sectorFadePixelValues[16] = {255,239,223,207,191,175,159,143,127,111,95,79,63,47,31,15};
volatile bool interruptAnimation = false;
Adafruit_WS2801 lightStrip;

void Animation_Library::setInterruptAnimation() {
  interruptAnimation = true;
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

byte determineColorValue(byte color, int weight){
  byte colorWeight = 0;
  if(weight == 0) {
    colorWeight = 0;
  } else if(weight == 1) {
    colorWeight = color;
  } else {
    colorWeight = color/weight;
  }

  return colorWeight;
}

uint32_t ColorByWeight(int colorValue, int colorWeight[]) {
  byte r;
  byte g;
  byte b;
  //
  // r = determineColorValue(rgb[0], colorWeight[0]);
  // g = determineColorValue(rgb[1], colorWeight[1]);
  // b = determineColorValue(rgb[2], colorWeight[2]);

  r = determineColorValue(colorValue, colorWeight[0]);
  g = determineColorValue(colorValue, colorWeight[1]);
  b = determineColorValue(colorValue, colorWeight[2]);

  return Color(r, g, b);
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void Animation_Library::rainbow(uint8_t wait) {
  int i, j;

  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    if(interruptAnimation) {
      break;
    }
    for (i=0; i < lightStrip.numPixels(); i++) {
      if(interruptAnimation) {
        break;
      }
      lightStrip.setPixelColor(i, Wheel( (i + j) % 255));
    }
    lightStrip.show();   // write all the pixels out
    delay(wait);
  }

  if(interruptAnimation) {
    interruptAnimation = false;
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
void Animation_Library::rainbowCycle(uint8_t wait) {
  int i, j;

  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    if(interruptAnimation) {
      break;
    }
    for (i=0; i < lightStrip.numPixels(); i++) {
      if(interruptAnimation) {
        break;
      }

      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / lightStrip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      lightStrip.setPixelColor(i, Wheel( ((i * 256 / lightStrip.numPixels()) + j) % 256) );
    }
    lightStrip.show();   // write all the pixels out
    delay(wait);
  }

  if(interruptAnimation) {
    interruptAnimation = false;
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < lightStrip.numPixels(); i++) {
      lightStrip.setPixelColor(i, c);
      lightStrip.show();
      delay(wait);
  }
}

void Animation_Library::setLightStrip(Adafruit_WS2801 strip){
  lightStrip = strip;
}

void Animation_Library::setPixelInSectorColorDoNotUpdateDisplay(int sectorIndex, int pixelIndex, uint32_t pixelColor){
  if(sectorIndex < NUM_SECTORS && pixelIndex < NUM_PIXELS_PER_SECTOR){
    lightStrip.setPixelColor(sectorIndices[sectorIndex][pixelIndex], pixelColor);
  }
}

void Animation_Library::setSectorFadeByPixel(int sectorIndex, int sectorPixel, int colorWeight[]){
  if(sectorIndex < NUM_SECTORS && sectorPixel < NUM_PIXELS_PER_SECTOR){
    for(int i=0; i<NUM_SECTORS; i++){
      int diffFromTargetSector = i - sectorIndex;

      int adjustedOffset = 0;
      if(diffFromTargetSector<0){
        adjustedOffset = ((diffFromTargetSector + NUM_SECTORS) * 2) + sectorPixel;
        setPixelInSectorColorDoNotUpdateDisplay(i, sectorPixel, ColorByWeight(sectorFadePixelValues[adjustedOffset], colorWeight));
        // setPixelInSectorColorDoNotUpdateDisplay(i, sectorPixel, Color(0, 0, sectorFadePixelValues[adjustedOffset]));
      } else {
        adjustedOffset = ((diffFromTargetSector) * 2) + sectorPixel;
        setPixelInSectorColorDoNotUpdateDisplay(i, sectorPixel, ColorByWeight(sectorFadePixelValues[adjustedOffset], colorWeight));
        // setPixelInSectorColorDoNotUpdateDisplay(i, sectorPixel, Color(0, 0, sectorFadePixelValues[adjustedOffset]));
      }
    }
  }
}


// sets each pixel per sector, somewha`t out of order, resulting in a glow effect.
void Animation_Library::granularSectorFadeCycle(int wait, int colorWeight[], bool isNestedFunction){
  for(int i=0; i<NUM_SECTORS;i++) {
    if(interruptAnimation) {
      break;
    }
    for(int j=0; j<NUM_PIXELS_PER_SECTOR; j++) {
      if(interruptAnimation) {
        break;
      }
      setSectorFadeByPixel(i, j, colorWeight);
      lightStrip.show();
      delay(wait);
    }
  }

  if(interruptAnimation && !isNestedFunction) {
    interruptAnimation = false;
  }
}

// sets each pixel per sector in order
void Animation_Library::linearSectorFadeCycle(int wait, uint32_t pixelColor){

}

// sets each pixel per sector, somewhat out of order, resulting in a glow effect.
void Animation_Library::granularSectorFadeCycleAllColors(int wait){
  int colorCycleWeight[3] = {0, 0, 0};
  for(int i=0; i<3; i++){
    if(interruptAnimation) {
      break;
    }

    for(int j=0; j<3; j++){
      if(j == i) {
        colorCycleWeight[j] = 1;
      } else {
        colorCycleWeight[j] = 0;
      }
    }
    granularSectorFadeCycle(wait, colorCycleWeight, true);
    setOnePixelColorAtATime(50, Color(0, 0, 0), true);
    delay(150);
  }

  if(interruptAnimation) {
    interruptAnimation = false;
  }
}

// sets each pixel per sector, somewhat out of order, resulting in a glow effect.
void Animation_Library::linearSectorCycleAllColors(int wait){
  int colorCycleWeight[3] = {0, 0, 0};
  for(int i=0; i<3; i++){
    if(interruptAnimation) {
      break;
    }

    for(int j=0; j<3; j++){
      if(j == i) {
        colorCycleWeight[j] = 1;
      } else {
        colorCycleWeight[j] = 0;
      }
    }
    setOnePixelColorAtATime(wait, ColorByWeight(255, colorCycleWeight), true);
    // setOnePixelColorAtATime(50, Color(0,0,0), true);
    // delay(150);
  }

  if(interruptAnimation) {
    interruptAnimation = false;
  }
}

// animation to play after reset
void Animation_Library::animationReset(int wait) {
  int num_flashes = 2;
  for (int i=0; i<num_flashes; i++){
    colorWipe(Color(255, 255, 255), 0);
    delay(150);
    colorWipe(Color(0, 0, 0), 0);
    delay(150);
  }
}

void Animation_Library::setSectorColorDoNotUpdateDisplay(int sectorIndex, uint32_t pixelColor){
  if(sectorIndex < NUM_SECTORS){
    for(int i=0; i<NUM_PIXELS_PER_SECTOR; i++){
      lightStrip.setPixelColor(sectorIndices[sectorIndex][i], pixelColor);
    }
  }
}

void Animation_Library::setOnePixelColorAtATime(int wait, uint32_t pixelColor, bool isNestedFunction) {
  for(int i=0; i<NUM_SECTORS; i++) {
    if(interruptAnimation) {
      break;
    }
    for(int j=0; j<NUM_PIXELS_PER_SECTOR; j++) {
      if(interruptAnimation) {
        break;
      }
      lightStrip.setPixelColor(sectorIndices[i][j], pixelColor);
      lightStrip.show();
      delay(wait);
    }
  }

  if(interruptAnimation && !isNestedFunction) {
    interruptAnimation = false;
  }
}

void Animation_Library::setSectorFade(int sectorIndex, uint32_t pixelColor){
  if(sectorIndex < NUM_SECTORS){
    for(int i=0; i<NUM_SECTORS; i++){
      int diffFromTargetSector = i - sectorIndex;

      if(diffFromTargetSector<0){
        setSectorColorDoNotUpdateDisplay(i, Color(0, 0, sectorFadeValues[diffFromTargetSector + NUM_SECTORS]));
        setSectorColorDoNotUpdateDisplay(i, pixelColor);
      } else {
        setSectorColorDoNotUpdateDisplay(i, Color(0, 0, sectorFadeValues[diffFromTargetSector]));
      }
    }
    lightStrip.show();
  }
}

void Animation_Library::sectorFadeCycle(int wait, uint32_t pixelColor){
  for(int i=0; i<NUM_SECTORS;i++){
    setSectorFade(i, pixelColor);
    delay(wait);
  }
}

void Animation_Library::setOneSectorAndClearOthers(int sectorIndex, uint32_t pixelColor){
  if(sectorIndex < NUM_SECTORS){
    for(int i=0; i<NUM_SECTORS; i++){
      if(i == sectorIndex){
        setSectorColorDoNotUpdateDisplay(i, pixelColor);
      }
      else{
        setSectorColorDoNotUpdateDisplay(i, 0);
      }
    }
    lightStrip.show();
  }
}

void Animation_Library::singleSectorCycle(uint8_t wait, uint32_t pixelColor){
  for(int i=0; i<NUM_SECTORS;i++){
    setOneSectorAndClearOthers(i, pixelColor);
    delay(wait);
  }
}

void Animation_Library::setOnePixelAndClearOthers(int pixelIndex, uint32_t pixelColor){
  if(pixelIndex < lightStrip.numPixels()){
    for(int i=0; i<lightStrip.numPixels(); i++){
      if(i == pixelIndex){
        lightStrip.setPixelColor(i, pixelColor);
      }
      else{
        lightStrip.setPixelColor(i, 0);
      }
    }
    lightStrip.show();
  }
}

uint32_t randomColor(){
  int r = rand() % 256;
  int g = rand() % 256;
  int b = rand() % 256;

  return Color(r, g ,b);
}

void Animation_Library::randomSetPixel(int wait, bool isNestedFunction){
  while(!interruptAnimation) {
    int randSectorIndex = rand() % NUM_SECTORS;
    int randPixelIndex = rand() % NUM_PIXELS_PER_SECTOR;

    lightStrip.setPixelColor(sectorIndices[randSectorIndex][randPixelIndex], randomColor());
    lightStrip.show();
    delay(wait);
  }

  if(interruptAnimation && !isNestedFunction) {
    interruptAnimation = false;
  }
}
