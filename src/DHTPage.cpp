#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "DHTPage.h"
#include "../icons/celsius.h"
#include "../icons/humidity.h"

// Screen resolution
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

// Thermometer icon dimensions
#define THERMOMETER_HEIGHT 50
#define THERMOMETER_WIDTH 50

// Humidity icon dimensions
#define HUMIDITY_HEIGHT 50
#define HUMIDITY_WIDTH 50

// Forward declaration of the function to draw the Celsius bitmap
void drawBitmapIcon(Adafruit_ST7789 &tft, int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h);

DHTPage::DHTPage(Adafruit_ST7789 &display)
    : tft(display), dht(DHT_PIN, DHT_TYPE) {}

void DHTPage::setup() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    dht.begin();
    
    // Draw the Celsius icon at the top of the screen (for initial setup or testing)
    drawCelsiusIcon();
    drawHumidityIcon(); // Draw the humidity icon for initial setup
}

float DHTPage::readMQ5() {
    return analogRead(MQ5_PIN); // Returns a value from 0 to 4095 for ESP32
}

String DHTPage::determineGasQuality(float mq5Percentage) {
    if (mq5Percentage < 25) return "Excellent";
    if (mq5Percentage < 35) return "Good";
    if (mq5Percentage < 55) return "OK";
    if (mq5Percentage < 65) return "Bad";
    return "Danger";
}

void DHTPage::update(bool forceRender) {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        updateDisplay("DHT Read Error");
        return;
    }

    // Read MQ5 gas quality
    float mq5Value = readMQ5();
    float mq5Percentage = (mq5Value / 4095.0) * 100;  // Convert to percentage
    String gasQuality = determineGasQuality(mq5Percentage);

    if (forceRender || 
        temperature != lastTemperature || 
        humidity != lastHumidity || 
        gasQuality != lastGasQuality) {
        updateDisplay((int)temperature, (int)humidity, gasQuality, forceRender);
    }
}

void DHTPage::updateDisplay(int temperature, int humidity, const String& gasQuality, bool forceRender) {
    // Draw the temperature icon and value at the top left of the screen
    int16_t iconX = 30; // Fixed horizontal position for the temperature icon
    int16_t iconY = 10; // Fixed vertical position near the top (10 pixels from the top)

    // Clear previous temperature value area and redraw if values change or forced render
    if (forceRender || temperature != lastTemperature) {
        tft.fillRect(iconX + THERMOMETER_WIDTH + 40, iconY, 100, 40, ST77XX_BLACK); // Clear area for temperature value
        drawBitmapIcon(tft, iconX, iconY, thermometer, THERMOMETER_WIDTH, THERMOMETER_HEIGHT); // Draw icon

        // Set text size and calculate position for temperature value
        tft.setTextSize(3); // Increase text size
        int16_t valueX = iconX + THERMOMETER_WIDTH + 40; // X position for value
        int16_t valueY = iconY + (THERMOMETER_HEIGHT / 2) - 20; // Y position for value

        // Draw temperature value next to the icon
        tft.setCursor(valueX, valueY);
        tft.print(temperature);
        tft.print(" C");
    }

    // Clear previous humidity value area and redraw if values change
    if (forceRender || humidity != lastHumidity) {
        int16_t humidityIconX = 30; // Fixed horizontal position for the humidity icon
        int16_t humidityIconY = 80; // Position humidity icon below the temperature display

        tft.fillRect(humidityIconX + HUMIDITY_WIDTH + 40, humidityIconY, 100, 40, ST77XX_BLACK); // Clear area for humidity value
        drawBitmapIcon(tft, humidityIconX, humidityIconY, humidityIcon, HUMIDITY_WIDTH, HUMIDITY_HEIGHT); // Draw icon

        // Set text size and calculate position for humidity value
        tft.setTextSize(3); // Increase text size
        int16_t humidityValueX = humidityIconX + HUMIDITY_WIDTH + 40; // X position for value
        int16_t humidityValueY = humidityIconY + (HUMIDITY_HEIGHT / 2) - 20; // Y position for value

        // Draw humidity value next to the icon
        tft.setCursor(humidityValueX, humidityValueY);
        tft.print(humidity);
        tft.print(" %");
    }

    // Clear and update air quality if it has changed or if forceRender is true
    if (forceRender || gasQuality != lastGasQuality) {
        tft.fillRect(10, SCREEN_HEIGHT - 80, 220, 80, ST77XX_BLACK); // Clear previous air quality area with color
        String airQualityLabel = "Air Quality:";
        String airQualityValue = gasQuality;

        // Calculate width for each line of text
        const uint8_t CHAR_WIDTH = 9; // Width based on font size
        uint16_t labelWidth = airQualityLabel.length() * CHAR_WIDTH * 2; // Width for the label
        uint16_t valueWidth = airQualityValue.length() * CHAR_WIDTH * 2; // Width for the value

        // Calculate positions to center the text
        int16_t labelCenterX = (SCREEN_WIDTH - labelWidth) / 2; // Center for label
        int16_t valueCenterX = (SCREEN_WIDTH - valueWidth) / 2; // Center for value

        // Draw air quality label and value
        tft.setCursor(labelCenterX, SCREEN_HEIGHT - 80); // Adjust vertical position for label
        tft.print(airQualityLabel); // Print air quality label
        tft.setCursor(valueCenterX, SCREEN_HEIGHT - 50); // Adjust vertical position for value
        tft.print(airQualityValue); // Print air quality value
    }

    // Store the last values
    lastTemperature = temperature; 
    lastHumidity = humidity; 
    lastGasQuality = gasQuality; 
}

void DHTPage::updateDisplay(const String& message) {
    tft.fillRect(10, 10, 220, 30, ST77XX_BLACK); 
    tft.setCursor(10, 10);
    tft.println(message);
}

// Function to draw the Celsius icon at the top
void DHTPage::drawCelsiusIcon() {
    // Use the drawBitmap function to draw the Celsius icon (for testing/setup)
    drawBitmapIcon(tft, 0, 0, thermometer, THERMOMETER_WIDTH, THERMOMETER_HEIGHT); // Place at top-left (0,0)
}

// Function to draw the humidity icon
void DHTPage::drawHumidityIcon() {
    // Use the drawBitmap function to draw the humidity icon (for testing/setup)
    drawBitmapIcon(tft, 0, HUMIDITY_HEIGHT + 10, humidityIcon, HUMIDITY_WIDTH, HUMIDITY_HEIGHT); // Place below the Celsius icon
}

// Reuse this function from your example to draw the bitmap
void drawBitmapIcon(Adafruit_ST7789 &tft, int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
    for (int16_t i = 0; i < h; i++) {
        for (int16_t j = 0; j < w; j++) {
            uint16_t color = pgm_read_word(&bitmap[i * w + j]);
            tft.drawPixel(x + j, y + i, color);
        }
    }
}

// Getter for temperature to be used in main for MQTT
float DHTPage::getTemperature() {
    return dht.readTemperature();
}

// Getter for humidity to be used in main for MQTT
float DHTPage::getHumidity() {
    return dht.readHumidity();
}

// Getter for gas quality to be used in main for MQTT
String DHTPage::getGasQuality() {
    float mq5Value = readMQ5();
    float mq5Percentage = (mq5Value / 4095.0) * 100;
    return determineGasQuality(mq5Percentage);
}

float DHTPage::getMQ5Percentage() {
    float mq5Value = readMQ5();
    return (mq5Value / 4095.0) * 100;  // Convert to percentage
}