#include "WiFiPage.h"

WiFiPage::WiFiPage(Adafruit_ST7789 &display) 
    : tft(display), lastSsid(""), lastIp(""), lastMac(""), lastRssi(0), lastCpuFreq(0), lastFreeMem("") {}

void WiFiPage::setup() {
    // Clear the screen and set initial text size/color
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
}

void WiFiPage::update(bool forceRender) {
    // Update the display with current Wi-Fi and ESP32 information
    displayInfo(forceRender);
}

void WiFiPage::displayInfo(bool forceRender) {
    // Retrieve current data
    String currentSsid = WiFi.SSID();          // Get SSID
    String currentIp = WiFi.localIP().toString();
    String currentMac = WiFi.macAddress();
    int currentRssi = WiFi.RSSI();
    int currentCpuFreq = ESP.getCpuFreqMHz();
    String currentFreeMem = formatMemory(ESP.getFreeHeap());

    // Wi-Fi Info Header
    tft.setTextSize(2);  // Set text size for the header
    tft.setCursor(10, 0);  // Position at the top of the screen
    tft.println("Wi-Fi Info:");
    tft.setTextSize(1);  // Set text size back to 1 for details

    // Update SSID
    if (forceRender || currentSsid != lastSsid) {
        tft.fillRect(10, 30, 220, 10, ST77XX_BLACK); // Clear space for SSID
        tft.setCursor(10, 30);
        tft.print("SSID: "); tft.println(currentSsid);
        lastSsid = currentSsid;
    }

    // Update IP Address
    if (forceRender || currentIp != lastIp) {
        tft.fillRect(10, 50, 220, 10, ST77XX_BLACK); // Clear space for IP
        tft.setCursor(10, 50);
        tft.print("IP: "); tft.println(currentIp);
        lastIp = currentIp;
    }

    // Update MAC Address
    if (forceRender || currentMac != lastMac) {
        tft.fillRect(10, 70, 220, 10, ST77XX_BLACK); // Clear space for MAC
        tft.setCursor(10, 70);
        tft.print("MAC: "); tft.println(currentMac);
        lastMac = currentMac;
    }

    // Update Signal Strength
    if (forceRender || currentRssi != lastRssi) {
        tft.fillRect(10, 90, 220, 10, ST77XX_BLACK); // Clear space for Signal Strength
        tft.setCursor(10, 90);
        tft.print("Signal: "); 
        tft.print(currentRssi); tft.print(" dBm (");
        tft.print(rssiToPercent(currentRssi)); tft.println("%)");
        lastRssi = currentRssi;
    }

    // Display ESP32 Info header
    tft.setTextSize(2);
    tft.setCursor(10, 130);  // Adjusted y-position to avoid overlapping
    tft.println("ESP32 Info:");
    tft.setTextSize(1);

    // Update CPU Frequency
    if (forceRender || currentCpuFreq != lastCpuFreq) {
        tft.fillRect(10, 160, 220, 10, ST77XX_BLACK); // Clear space for CPU Frequency
        tft.setCursor(10, 160);
        tft.print("CPU: "); tft.print(currentCpuFreq); tft.println(" MHz");
        lastCpuFreq = currentCpuFreq;
    }

    // Update Free Memory
    if (forceRender || currentFreeMem != lastFreeMem) {
        tft.fillRect(10, 180, 220, 10, ST77XX_BLACK); // Clear space for Free Memory
        tft.setCursor(10, 180);
        tft.print("Free Mem: "); tft.print(currentFreeMem); tft.println(" bytes");
        lastFreeMem = currentFreeMem;
    }
}

// Convert RSSI to percentage
int WiFiPage::rssiToPercent(int rssi) {
    if (rssi <= -100) return 0;
    else if (rssi >= -50) return 100;
    else return 2 * (rssi + 100);
}

// Format memory in a human-readable format
String WiFiPage::formatMemory(size_t bytes) {
    if (bytes >= 1024 * 1024) {
        return String(bytes / (1024 * 1024)) + " MB";
    } else if (bytes >= 1024) {
        return String(bytes / 1024) + " KB";
    } else {
        return String(bytes) + " bytes";
    }
}
