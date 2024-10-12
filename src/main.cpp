#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "OTAUpdate.h"
#include "WebServerHandler.h"
#include "slideshow.h"
#include "DHTPage.h"
#include "WiFiPage.h"
#include "WeatherPage.h"
#include "MQTTHandler.h"
#include "secrets.h"
#include "HCSR04Sensor.h"

// Pin definitions for ST7789 display
#define TFT_CS     5    
#define TFT_RST    22   
#define TFT_DC     4    
#define TFT_MOSI   23   
#define TFT_CLK    18   
#define BACKLIGHT_PIN 21
#define BUTTON_PIN 15  // GPIO pin for the button

// Instantiate the HC-SR04 sensor
HCSR04Sensor ultrasonicSensor(14, 27);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
String version = "v0.9.0";

// Enumeration for pages
enum class Page {
    SLIDESHOW,
    DHT,
    WEATHER,
    WIFI
};

// Global variables
bool forceRender = true;
Page pages[] = {Page::SLIDESHOW, Page::DHT, Page::WEATHER, Page::WIFI};
int pageIndex = 1;

// Web Server setup
WebServer server(80);

// Button timing constants
const unsigned long DOUBLE_CLICK_DELAY = 300;  
const unsigned long LONG_PRESS_THRESHOLD = 1500;  
const unsigned long PAGE_SWITCH_INTERVAL = 45000; // 45 seconds
const unsigned long SLIDESHOW_DELAY = 180000;   // 3 minutes
const unsigned long DIM_DELAY = 300000;          // 5 minutes
const unsigned long SCREEN_OFF_DELAY = 600000;   // 10 minutes

// Timer variables for sensor reading
const unsigned long ULTRASONIC_READ_INTERVAL = 500;
unsigned long lastSensorReadTime = 0;

// Button state and timing variables
bool buttonPressed = false;
unsigned long pressStartTime = 0;
unsigned long lastActionTime = 0;
unsigned long lastClickTime = 0;
unsigned long lastSwitchTime = 0;
bool isDimmed = false;
bool slideshowUserInitiated = false;

// Page objects
DHTPage dhtPage(tft);
WiFiPage wifiPage(tft);
WeatherPage weatherPage(tft, openWeatherApiKey, "Munich");

// Function declarations
void setupDisplay();
void connectToWiFi();
void initializeComponents();
void updateDisplay();
void changePage(int direction);
void handleButtonPress();
void handleButtonRelease();
void setBacklight(uint8_t brightness);
void publishAllSensorData();
void readUltrasonicSensor();
void checkPageSwitching();
void checkInactivity();
void checkButtonLongPress();

void setup() {
    Serial.begin(115200);
    setupDisplay();
    connectToWiFi();
    initializeComponents();
}

void loop() {
    handleOTA();       
    handleWebServer(); 
    handleButtonPress();
    updateDisplay();
    maintainMQTTConnection(mqttUser, mqttPassword);
    publishAllSensorData();
    readUltrasonicSensor();
    checkPageSwitching();
    checkInactivity();
}

// Setup display settings
void setupDisplay() {
    tft.init(240, 240);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(10, 10);
    tft.println("Initializing...");
    pinMode(BACKLIGHT_PIN, OUTPUT);
    setBacklight(100);  // 100% brightness
}

// Connect to WiFi network
void connectToWiFi() {
    WiFi.hostname("weatherstation-kitchen");
    WiFi.begin(ssid, wifiPass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        tft.setCursor(10, 40);
        tft.println("Connecting to WiFi...");
    }
}

// Initialize components
void initializeComponents() {
    setupOTA(otaPassword);
    setupWebServer(webAuthUser, webAuthPass);
    setupMQTT(mqttUser, mqttPassword); 
    ultrasonicSensor.begin(); // Initialize the ultrasonic sensor
    initSlideshow(tft);
    dhtPage.setup();
    wifiPage.setup();
    weatherPage.setup();
    pinMode(BUTTON_PIN, INPUT_PULLUP); 
}

// Update the display based on the current page
void updateDisplay() {
    switch (pages[pageIndex]) {
        case Page::SLIDESHOW:
            updateSlideshow(tft, forceRender); 
            break;
        case Page::DHT:
            dhtPage.update(forceRender); 
            break;
        case Page::WEATHER:
            weatherPage.update(false); 
            break;
        case Page::WIFI:
            wifiPage.update(forceRender); 
            break;
    }
    forceRender = false;  // Reset flag after updating display
}

// Change the current page based on direction (-1 or +1)
void changePage(int direction) {
    pageIndex += direction;
    if (pageIndex >= sizeof(pages) / sizeof(pages[0])) 
        pageIndex = 0; 
    else if (pageIndex < 0) 
        pageIndex = sizeof(pages) / sizeof(pages[0]) - 1; 

    tft.fillScreen(ST77XX_BLACK);  
    forceRender = true;  
    updateDisplay();  
}

// Handle button press logic
void handleButtonPress() {
    if (digitalRead(BUTTON_PIN) == LOW) { 
        if (!buttonPressed) {
            buttonPressed = true;
            pressStartTime = millis();
            // Reset backlight and inactivity timer
            if (isDimmed) {
                setBacklight(100);  // Set back to full brightness
                isDimmed = false;
            }
            lastActionTime = millis();  
        }
    } else { 
        if (buttonPressed) {
            buttonPressed = false;
            handleButtonRelease();
        }
    }
    // Check for long press
    checkButtonLongPress();
}

// Handle button release logic
void handleButtonRelease() {
    unsigned long pressDuration = millis() - pressStartTime;
    if (pressDuration < LONG_PRESS_THRESHOLD) {
        // Double-click detection
        if (millis() - lastClickTime < DOUBLE_CLICK_DELAY) {
            pageIndex = static_cast<int>(Page::WIFI);
            tft.fillScreen(ST77XX_BLACK);
            forceRender = true;
            updateDisplay();
        } else {
            // Single click logic for page navigation
            if (pageIndex == static_cast<int>(Page::SLIDESHOW) || pageIndex == static_cast<int>(Page::WIFI)) {
                pageIndex = static_cast<int>(Page::DHT);
            } else if (pages[pageIndex] == Page::DHT) {
                pageIndex = static_cast<int>(Page::WEATHER);
            } else if (pages[pageIndex] == Page::WEATHER) {
                pageIndex = static_cast<int>(Page::DHT);
            } else {
                pageIndex = static_cast<int>(Page::DHT);  // Default to DHT page
            }
            tft.fillScreen(ST77XX_BLACK);
            forceRender = true;
            updateDisplay();
        }
        lastClickTime = millis(); // Update last click time
    } else if (pressDuration >= LONG_PRESS_THRESHOLD) {
        // Long press: go to slideshow page
        pageIndex = static_cast<int>(Page::SLIDESHOW);
        slideshowUserInitiated = true;
        tft.fillScreen(ST77XX_BLACK);
        forceRender = true;
        updateDisplay();
    }

    // Refresh weather data if on Weather page
    if (pages[pageIndex] == Page::WEATHER) {
        weatherPage.update(true);
    }

    // Reset inactivity timer
    lastActionTime = millis();  
}

// Check for long press duration
void checkButtonLongPress() {
    if (buttonPressed && (millis() - pressStartTime >= LONG_PRESS_THRESHOLD)) {
        lastActionTime = millis();  // Reset inactivity timer for long press
    }
}

// Function to set the backlight brightness in percentage (0-100%)
void setBacklight(uint8_t percent) {
    // Clamp the value to ensure it's between 0 and 100
    if (percent > 100) percent = 100;

    // Convert percentage (0-100) to PWM range (0-255)
    uint8_t brightness = (percent / 100.0) * 255;
    analogWrite(BACKLIGHT_PIN, brightness);  // Set backlight brightness
}

// Publish all sensor data
void publishAllSensorData() {
    float dhtTemp = dhtPage.getTemperature();
    float dhtHumidity = dhtPage.getHumidity();
    float mq5Percentage = dhtPage.getMQ5Percentage();
    String gasQuality = dhtPage.getGasQuality();
    int wifiSignalStrength = WiFi.RSSI();  // Wi-Fi signal strength
    String ipAddress = WiFi.localIP().toString();  // Get IP address
    String macAddress = WiFi.macAddress();  // Get MAC address
    int cpuFreq = ESP.getCpuFreqMHz();  // Get CPU frequency
    String freeMem = wifiPage.formatMemory(ESP.getFreeHeap());  // Get free memory

    // Pass all sensor data to the MQTTHandler for processing and publishing
    processAndPublishSensorData(dhtTemp, dhtHumidity, mq5Percentage, gasQuality, wifiSignalStrength, 
                                ipAddress, macAddress, cpuFreq, freeMem);
}

// Read ultrasonic sensor and handle actions based on distance
void readUltrasonicSensor() {
    if (millis() - lastSensorReadTime >= ULTRASONIC_READ_INTERVAL) {
        lastSensorReadTime = millis(); // Update the last read time
        long distance = ultrasonicSensor.readDistance(); // Read distance

        if (distance > 0 && distance < 60) {
            lastActionTime = millis(); // Reset inactivity timer
            if (isDimmed) {
                setBacklight(100);  // Set backlight to 100%
                isDimmed = false;
            }

            // Check if the current page is not DHT or is on the Weather page
            if (!slideshowUserInitiated && (pages[pageIndex] == Page::WEATHER || pages[pageIndex] != Page::DHT)) {
                pageIndex = static_cast<int>(Page::DHT); // Switch to DHT page
                lastSwitchTime = millis();
                tft.fillScreen(ST77XX_BLACK);  
                forceRender = true; 
                updateDisplay();  // Update display to show DHT page
            }
        }
    }
}

// Check for automatic page switching
void checkPageSwitching() {
    if ((pages[pageIndex] == Page::DHT || pages[pageIndex] == Page::WEATHER) && 
        (millis() - lastSwitchTime >= PAGE_SWITCH_INTERVAL)) {
        // Switch to the other page
        if (pages[pageIndex] == Page::DHT) {
            pageIndex = static_cast<int>(Page::WEATHER);
            tft.fillScreen(ST77XX_BLACK);  
            weatherPage.update(true); 
        } else {
            pageIndex = static_cast<int>(Page::DHT);
            tft.fillScreen(ST77XX_BLACK);  
            dhtPage.update(true); 
        }
        lastSwitchTime = millis(); // Update last switch time
    }
}

// Check for inactivity to dim the backlight or activate slideshow
void checkInactivity() {
    if ((millis() - lastActionTime >= DIM_DELAY) && !isDimmed) {
        setBacklight(10);  // Dim to 10%
        isDimmed = true;   // Track that it's dimmed
    }

    if ((millis() - lastActionTime >= SLIDESHOW_DELAY) && (pages[pageIndex] != Page::SLIDESHOW)) {
        pageIndex = static_cast<int>(Page::SLIDESHOW); // Switch to slideshow
        tft.fillScreen(ST77XX_BLACK);  
        forceRender = true;
        updateDisplay();  // Update display to show slideshow
    }

    if ((millis() - lastActionTime >= SCREEN_OFF_DELAY)) {
        setBacklight(0);  // Turn off the backlight
    }
}
