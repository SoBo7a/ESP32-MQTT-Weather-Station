#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Function prototypes
void initSlideshow(Adafruit_ST7789 &tft);
void updateSlideshow(Adafruit_ST7789 &tft, bool forceRender);

#endif
