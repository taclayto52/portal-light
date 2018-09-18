#ifndef __ANIMATION_LIBRARY__
#define __ANIMATION_LIBRARY__

#include <Arduino.h>
#include <Adafruit_WS2801.h>

class Animation_Library {
  public:
    void granularSectorFadeCycle(int wait, int colorWeight[], bool isNestedFunction),
     granularSectorFadeCycleAllColors(int wait),
     rainbow(uint8_t wait),
     rainbowCycle(uint8_t wait),
     linearSectorFadeCycle(int wait, uint32_t pixelColor),
     sectorFadeCycle(int wait, uint32_t pixelColor),
     singleSectorCycle(uint8_t wait, uint32_t pixelColor),
     setOneSectorAndClearOthers(int sectorIndex, uint32_t pixelColor),
     setSectorFade(int sectorIndex, uint32_t pixelColor),
     setSectorFadeByPixel(int sectorIndex, int sectorPixel, int colorWeight[]),
     setPixelInSectorColorDoNotUpdateDisplay(int sectorIndex, int pixelIndex, uint32_t pixelColor),
     setSectorColorDoNotUpdateDisplay(int sectorIndex, uint32_t pixelColor),
     setOnePixelAndClearOthers(int pixelIndex, uint32_t pixelColor),
     setLightStrip(Adafruit_WS2801 strip),
     animationReset(int wait),
     setInterruptAnimation(),
     setOnePixelColorAtATime(int wait, uint32_t pixelColor, bool isNestedFunction),
     linearSectorCycleAllColors(int wait),
     randomSetPixel(int wait, bool isNestedFunction);
};

#endif
