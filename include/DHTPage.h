#ifndef DHTPAGE_H
#define DHTPAGE_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <DHT.h>

// DHT22 configuration
#define DHT_PIN    32
#define DHT_TYPE   DHT22

// MQ-5 configuration
#define MQ5_PIN    34

class DHTPage {
public:
    DHTPage(Adafruit_ST7789 &display);
    void setup();
    void update(bool forceRender);

    // Getters to retrieve the sensor data
    float getTemperature();  // Getter for temperature
    float getHumidity();     // Getter for humidity
    String getGasQuality();  // Getter for gas quality
    float getMQ5Percentage();

private:
    Adafruit_ST7789 &tft; // Reference to the display
    DHT dht;

    // Variables to store previous readings
    int lastTemperature = -999;
    int lastHumidity = -999;
    String lastGasQuality = "Unknown";

    float readMQ5();
    String determineGasQuality(float mq5Percentage);
    void updateDisplay(int temperature, int humidity, const String& gasQuality, bool forceRender);
    void updateDisplay(const String& message);

    // Declaration of the functions to draw the icons
    void drawCelsiusIcon();  
    void drawHumidityIcon();   

    // Helper function to draw bitmaps
    void drawBitmap(Adafruit_ST7789 &tft, int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h);
};

#endif // DHTPAGE_H
