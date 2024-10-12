#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "slideshow.h"
#include "../images/herbst.h"
#include "../images/hills.h"
#include "../images/seerosen.h"
#include "../images/wald.h"

// Forward declaration of drawBitmap
void drawBitmap(Adafruit_ST7789 &tft, int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h);

// Define images array
const uint16_t* images[] = {herbst, hills, seerosen, wald}; // Add your images here
const int numImages = sizeof(images) / sizeof(images[0]); // Number of images

// Variables for slideshow
unsigned long previousMillis = 0; // Stores last time image was updated
const long interval = 180000; // Interval for image change (3 minutes)
int currentImageIndex = random(numImages); // Current image index

void initSlideshow(Adafruit_ST7789 &tft) {
    // Initialize the display
    tft.init(240, 240); // Initialize ST7789 with width and height
    tft.fillScreen(ST77XX_BLACK); // Fill screen with black color
}

void updateSlideshow(Adafruit_ST7789 &tft, bool forceRender) {
    unsigned long currentMillis = millis(); // Get the current time

    // Check if 30 seconds have passed or a page switch has occurred
    if (forceRender || (currentMillis - previousMillis >= interval)) {
        previousMillis = currentMillis; // Save the last time an image was updated

        // Update the current image index if not forced render
        if (!forceRender) {
            currentImageIndex = (currentImageIndex + 1) % numImages; // Cycle through images
        } else {
            currentImageIndex = random(numImages);
        }

        // Display the new image
        drawBitmap(tft, 0, 0, images[currentImageIndex], 240, 240);
    }
}

void drawBitmap(Adafruit_ST7789 &tft, int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
    for (int16_t i = 0; i < h; i++) {
        for (int16_t j = 0; j < w; j++) {
            uint16_t color = pgm_read_word(&bitmap[i * w + j]);
            tft.drawPixel(x + j, y + i, color);
        }
    }
}
