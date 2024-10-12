# ESP32 MQTT Weather Station

## Overview

This project implements a weather station using an ESP32 microcontroller that collects environmental data such as temperature, humidity, and gas quality. The data is published to an MQTT broker, allowing real-time monitoring and control via MQTT clients. This project leverages the PlatformIO IDE for easy development and deployment.

## Features

- Measures temperature and humidity using DHT sensors
- Monitors air quality with an MQ5 gas sensor
- Reports Wi-Fi signal strength and device information (IP and MAC address)
- Publishes data to an MQTT broker
- Efficiently publishes only changed data to minimize network traffic
- Supports Over-The-Air (OTA) updates for easy firmware upgrades
- Provides a web server for real-time monitoring and configuration

## Hardware Requirements

- ESP32 development board
- DHT sensor (e.g., DHT11 or DHT22)
- MQ5 gas sensor
- Jumper wires
- Breadboard (optional)

## Software Requirements

- [PlatformIO](https://platformio.org/) (IDE extension for Visual Studio Code or standalone)
- MQTT broker (e.g., [Mosquitto](https://mosquitto.org/) or [Eclipse Paho](https://www.eclipse.org/paho/))

## Installation

1. **Clone the repository:**

```bash
git clone https://github.com/SoBo7a/esp32-mqtt-weather-station.git
cd esp32-mqtt-weather-station
```

2. **Open the project in PlatformIO:**

- Launch Visual Studio Code and open the cloned project folder.
- Install the required libraries if prompted.

3. **Configure Wi-Fi and MQTT settings:**

- Open the secrets.h File and update with your data 
- Change the OTA Password in the platformio.ini
- Open src/MQTTHandler.cpp and set your MQTT broker details.
```bash
const char* mqttServer = "your.mqtt.server";
const int mqttPort = 1883;
```

4. Build and upload the code:

- Connect your ESP32 board to your computer.
- Select the correct environment for your board in PlatformIO (e.g., esp32dev).
- Build and upload the project by clicking on the "Upload" button in the PlatformIO toolbar.

## Usage
- After uploading the code, open the Serial Monitor in PlatformIO to view the output.
- The ESP32 will connect to your Wi-Fi network and the MQTT broker.
- You can monitor the published data by subscribing to the respective MQTT topics using an MQTT client (e.g., MQTT Explorer, MQTT.fx).

### Topics
- Temperature: ```home/dht/temperature```
- Humidity: ```home/dht/humidity```
- MQ5 Gas Percentage: ```home/mq5/percentage```
- Gas Quality: ```home/mq5/gas```
- Wi-Fi Signal Strength: ```home/wifi/signal```
- IP Address: ```home/wifi/ip```
- MAC Address: ```home/wifi/mac```
- CPU Frequency: ```home/esp32/cpu_freq```
- Free Memory: ```home/esp32/free_mem```