// WiFiPage.h
#ifndef WIFIPAGE_H
#define WIFIPAGE_H

#include <Adafruit_ST7789.h>
#include <WiFi.h>

class WiFiPage {
public:
    WiFiPage(Adafruit_ST7789 &display);
    void setup();
    void update(bool forceRender = false);
    String formatMemory(size_t bytes);

private:
    Adafruit_ST7789 &tft;
    
    String lastSsid;
    String lastIp;
    String lastMac;
    int lastRssi;
    int lastCpuFreq;
    String lastFreeMem;
    
    void displayInfo(bool forceRender);
    int rssiToPercent(int rssi);
};

#endif
