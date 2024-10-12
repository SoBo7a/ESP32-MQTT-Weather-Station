#include <ArduinoOTA.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

extern Adafruit_ST7789 tft;  // External reference to the display object

void setupOTA(const char* otaPassword) {
    ArduinoOTA.setPassword(otaPassword); // Set OTA password
    ArduinoOTA.setHostname("weatherstation-kitchen");
    ArduinoOTA.begin();

    // Set up OTA callbacks
    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        tft.fillScreen(ST77XX_BLACK);
        tft.setCursor(10, 10);
        tft.println("Updating...");
        tft.setCursor(10, 50);
        tft.print("Progress: ");
        tft.print((progress / (total / 100)));
        tft.println("%");
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });
}

void handleOTA() {
    ArduinoOTA.handle();  // Handles OTA updates
}
