#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>
#include <vector>
#include <string>

// Data structure to hold sensor topic and its value
struct SensorData {
    const char* topic;  // MQTT topic
    String value;       // Sensor value to publish
};

// Function to set up the MQTT connection
void setupMQTT(const char* user, const char* password);

// Function to maintain MQTT connection with user credentials
void maintainMQTTConnection(const char* user, const char* password);


// Function to process sensor data, compare with last values, and publish if changes are detected
void processAndPublishSensorData(float dhtTemp, float dhtHumidity, float mq5Percentage, const String& gasQuality, 
                                int wifiSignalStrength, const String& ipAddress, const String& macAddress, 
                                int cpuFreq, const String& freeMem);

#endif
