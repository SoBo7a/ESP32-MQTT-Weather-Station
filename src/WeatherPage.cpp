#include "WeatherPage.h"

// OpenWeatherMap API endpoint
const char *apiEndpoint = "http://api.openweathermap.org/data/2.5/weather?q=";

WeatherPage::WeatherPage(Adafruit_ST7789 &display, const char *apiKey, const char *city)
    : tft(display), apiKey(apiKey), city(city), temperature(0), feelsLike(0), humidity(0), lastUpdateTime(0) {}

void WeatherPage::setup() {
    getWeather();  // Fetch initial weather data
}

void WeatherPage::update(bool forceRender) {
    unsigned long currentTime = millis();

    // Update weather data if the interval has passed
    if (currentTime - lastUpdateTime >= updateInterval) {
        float oldTemperature = temperature;  // Store previous values for comparison
        float oldFeelsLike = feelsLike;
        float oldHumidity = humidity;

        getWeather();  // Fetch new weather data
        lastUpdateTime = currentTime;  // Update the last update time

        // Only display if any weather value has changed
        if (temperature != oldTemperature || feelsLike != oldFeelsLike || humidity != oldHumidity) {
            displayWeather();
        }
    }

    // Force render if requested
    if (forceRender) {
        displayWeather();
    }
}

void WeatherPage::getWeather() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        String url = String(apiEndpoint) + city + "&appid=" + apiKey + "&units=metric";
        Serial.println("Requesting URL: " + url);
        
        http.begin(url);
        int httpCode = http.GET();

        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println("Response:\n" + payload);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                weatherDescription = doc["weather"][0]["description"].as<String>();
                temperature = doc["main"]["temp"];
                feelsLike = doc["main"]["feels_like"];
                humidity = doc["main"]["humidity"];
            } else {
                Serial.println("Failed to parse JSON!");
            }
        } else {
            Serial.println("Error in HTTP request: " + String(httpCode));
        }

        http.end();
    } else {
        Serial.println("WiFi not connected!");
    }
}

const uint16_t* WeatherPage::getWeatherIcon() {
    // Find the corresponding icon for the weather description
    for (const auto &mapping : weatherIcons) {
        if (weatherDescription.indexOf(mapping.description) >= 0) {
            return mapping.icon;  // Return matching icon
        }
    }

    return sun;  // Default icon
}

void WeatherPage::displayWeather() {
    tft.fillScreen(ST77XX_BLACK);  // Clear the screen

    // Display the weather icon with stretching
    const uint16_t* icon = getWeatherIcon();
    drawWeatherIcon(tft, 80, 5, icon, 50, 50, 1.5);

    // Draw Temperature icon and value
    int16_t tempIconX = 30; // Fixed horizontal position for the temperature icon
    int16_t tempIconY = 90; // Position for temperature icon
    drawWeatherIcon(tft, tempIconX, tempIconY, thermometer, THERMOMETER_WIDTH, THERMOMETER_HEIGHT, 1);

    // Display temperature value
    tft.setTextSize(3);  // Increased text size
    tft.setCursor(tempIconX + THERMOMETER_WIDTH + 15, tempIconY + 10); // Adjust cursor position
    tft.print(temperature);
    tft.print(" C");

    // Draw Humidity icon and value
    int16_t humidityIconX = 30; 
    int16_t humidityIconY = tempIconY + THERMOMETER_HEIGHT + 20; 
    drawWeatherIcon(tft, humidityIconX, humidityIconY, humidityIcon, HUMIDITY_WIDTH, HUMIDITY_HEIGHT, 1); // No scaling

    // Display humidity value
    tft.setCursor(humidityIconX + HUMIDITY_WIDTH + 15, humidityIconY + 10); 
    tft.print(humidity);
    tft.print(" %");
}

// Function to draw bitmap with non-integer scaling
void drawWeatherIcon(Adafruit_ST7789 &tft, int16_t x, int16_t y, const uint16_t *bitmap, int16_t originalW, int16_t originalH, float scale) {
    for (int16_t i = 0; i < originalH; i++) {
        for (int16_t j = 0; j < originalW; j++) {
            uint16_t color = pgm_read_word(&bitmap[i * originalW + j]);
            // Calculate scaled pixel position
            int16_t scaledX = x + j * scale;
            int16_t scaledY = y + i * scale;

            // Draw the pixel for the non-integer scale
            for (float k = 0; k < scale; k += 1.0) {
                for (float l = 0; l < scale; l += 1.0) {
                    // Draw a pixel at the scaled position
                    tft.drawPixel(scaledX + l, scaledY + k, color);
                }
            }
        }
    }
}
