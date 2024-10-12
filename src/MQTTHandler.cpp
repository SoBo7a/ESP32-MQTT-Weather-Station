#include "MQTTHandler.h"
#include <WiFi.h>
#include <PubSubClient.h>

// MQTT Broker settings
const char* mqttServer = "192.168.178.82";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Previous sensor values for comparison
float lastDHTTemp = -100.0;
float lastDHTHumidity = -100.0;
float lastMQ5Percentage = -100.0;
String lastGasQuality = "Unknown";
int lastWiFiSignalStrength = 0;
String lastIPAddress = "";
String lastMACAddress = "";
int lastCPUFreq = 0;
String lastFreeMem = "";

// Function to connect to the MQTT broker
void setupMQTT(const char* user, const char* password) {
    client.setServer(mqttServer, mqttPort);

    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("WeatherStationClient", user, password)) {
            Serial.println("connected");
        } else {
            Serial.print("failed with state ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

// Function to maintain MQTT connection
void maintainMQTTConnection(const char* user, const char* password) {
    if (!client.connected()) {
        setupMQTT(user, password);  // Reconnect if disconnected
    }
    client.loop();  // Regularly call this to process MQTT messages
}

// Function to publish single sensor data to MQTT
void publishSingleSensorData(const char* topic, const String& value) {
    if (client.connected()) {
        client.publish(topic, value.c_str(), true);  // Retain the message
    }
}

// Function to process sensor data, compare with previous values, and publish only changed ones
void processAndPublishSensorData(float dhtTemp, float dhtHumidity, float mq5Percentage, const String& gasQuality, 
                                int wifiSignalStrength, const String& ipAddress, const String& macAddress, 
                                int cpuFreq, const String& freeMem) {

    // Temperature
    if (abs(dhtTemp - lastDHTTemp) > 0.1) {
        publishSingleSensorData("home/dht/temperature", String(dhtTemp));
        lastDHTTemp = dhtTemp;
    }

    // Humidity
    if (abs(dhtHumidity - lastDHTHumidity) > 1.0) {
        publishSingleSensorData("home/dht/humidity", String(dhtHumidity));
        lastDHTHumidity = dhtHumidity;
    }

    // MQ5 Gas Percentage
    if (abs(mq5Percentage - lastMQ5Percentage) > 2.5) { 
        publishSingleSensorData("home/mq5/percentage", String(mq5Percentage));
        lastMQ5Percentage = mq5Percentage;
    }

    // Gas quality
    if (gasQuality != lastGasQuality) {
        publishSingleSensorData("home/mq5/gas", gasQuality);
        lastGasQuality = gasQuality;
    }

    // Wi-Fi Signal Strength
    if (abs(wifiSignalStrength - lastWiFiSignalStrength) > 3) {
        publishSingleSensorData("home/wifi/signal", String(wifiSignalStrength));
        lastWiFiSignalStrength = wifiSignalStrength;
    }

    // IP Address
    if (ipAddress != lastIPAddress) {
        publishSingleSensorData("home/wifi/ip", ipAddress);
        lastIPAddress = ipAddress;
    }

    // MAC Address
    if (macAddress != lastMACAddress) {
        publishSingleSensorData("home/wifi/mac", macAddress);
        lastMACAddress = macAddress;
    }

    // CPU Frequency
    if (cpuFreq != lastCPUFreq) {
        publishSingleSensorData("home/esp32/cpu_freq", String(cpuFreq));
        lastCPUFreq = cpuFreq;
    }

    // Free Memory
    if (abs(freeMem.toInt() - lastFreeMem.toInt()) >= 5120) { // 5KB = 5120 bytes
        publishSingleSensorData("home/esp32/free_mem", freeMem);
        lastFreeMem = freeMem;
    }
}
