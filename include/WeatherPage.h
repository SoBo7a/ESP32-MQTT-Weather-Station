#ifndef WEATHER_PAGE_H
#define WEATHER_PAGE_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "../icons/celsius.h"
#include "../icons/humidity.h"

// Include your weather icon arrays
#include "../icons/clouds_broken.h"
#include "../icons/clouds_few.h"
#include "../icons/clouds_scattered.h"
#include "../icons/mist.h"
#include "../icons/rainy.h"
#include "../icons/snowflake.h"
#include "../icons/sun.h"
#include "../icons/thunderstorm.h"

// Structure for mapping weather descriptions to icons
struct WeatherIconMapping {
    const char *description;
    const uint16_t *icon;
};

// Define all weather conditions and their associated icons
const WeatherIconMapping weatherIcons[] = {
    {"clear sky", sun},
    {"few clouds", clouds_few},
    {"scattered clouds", clouds_scattered},
    {"broken clouds", clouds_broken},
    {"overcast clouds", clouds_scattered},
    {"light rain", rainy},
    {"moderate rain", rainy},
    {"heavy intensity rain", rainy},
    {"very heavy rain", rainy},
    {"extreme rain", rainy},
    {"shower rain", rainy},
    {"thunderstorm with light rain", thunderstorm},
    {"thunderstorm with rain", thunderstorm},
    {"thunderstorm with heavy rain", thunderstorm},
    {"light thunderstorm", thunderstorm},
    {"heavy thunderstorm", thunderstorm},
    {"ragged thunderstorm", thunderstorm},
    {"thunderstorm with light drizzle", thunderstorm},
    {"thunderstorm with drizzle", thunderstorm},
    {"thunderstorm with heavy drizzle", thunderstorm},
    {"light intensity drizzle", rainy},
    {"drizzle", rainy},
    {"heavy intensity drizzle", rainy},
    {"light intensity drizzle rain", rainy},
    {"drizzle rain", rainy},
    {"heavy intensity drizzle rain", rainy},
    {"shower rain and drizzle", rainy},
    {"heavy shower rain and drizzle", rainy},
    {"shower drizzle", rainy},
    {"light snow", snowflake},
    {"snow", snowflake},
    {"heavy snow", snowflake},
    {"sleet", snowflake},  // Assuming a new icon `snowy` for sleet
    {"light shower sleet", snowflake},
    {"shower sleet", snowflake},
    {"light rain and snow", snowflake},
    {"rain and snow", snowflake},
    {"light shower snow", snowflake},
    {"shower snow", snowflake},
    {"heavy shower snow", snowflake},
    {"mist", mist},
    {"smoke", mist},
    {"haze", mist},
    {"dust", mist},
    {"fog", mist},
    {"sand", mist},
    {"ash", mist},
    {"squalls", mist},
    {"tornado", thunderstorm}
};

class WeatherPage {
public:
    WeatherPage(Adafruit_ST7789 &display, const char *apiKey, const char *city);
    void setup();
    void update(bool forceRender);

private:
    Adafruit_ST7789 &tft;
    const char *apiKey;
    const char *city;
    String weatherDescription;
    float temperature;
    float feelsLike;
    float humidity;

    unsigned long lastUpdateTime;
    const unsigned long updateInterval = 300000;  // 5 minutes in milliseconds

    void getWeather();
    void displayWeather();
    const uint16_t* getWeatherIcon();  // Helper to determine which icon to display
};

// Function to draw the weather icon bitmap (declaration)
void drawWeatherIcon(Adafruit_ST7789 &tft, int16_t x, int16_t y, const uint16_t *bitmap, int16_t originalW, int16_t originalH, float scale);

#endif // WEATHER_PAGE_H
