#ifndef HCSR04SENSOR_H
#define HCSR04SENSOR_H

#include <Arduino.h>

class HCSR04Sensor {
public:
    HCSR04Sensor(int trigPin, int echoPin);
    void begin();
    long readDistance();

private:
    int _trigPin;
    int _echoPin;
};

#endif // HCSR04SENSOR_H
