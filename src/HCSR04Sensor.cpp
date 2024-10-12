#include "HCSR04Sensor.h"

HCSR04Sensor::HCSR04Sensor(int trigPin, int echoPin)
    : _trigPin(trigPin), _echoPin(echoPin) {}

void HCSR04Sensor::begin() {
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
}

long HCSR04Sensor::readDistance() {
    long duration, distance;

    // Repeat the process until a valid distance is read (not 0)
    do {
        // Clear the Trig pin
        digitalWrite(_trigPin, LOW);
        delayMicroseconds(2);

        // Set the Trig pin HIGH for 10 microseconds
        digitalWrite(_trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(_trigPin, LOW);

        // Read the Echo pin
        duration = pulseIn(_echoPin, HIGH);

        // Calculate distance in cm
        distance = duration * 0.034 / 2; // Speed of sound is 0.034 cm/µs

        // If the distance is 0, repeat the process
    } while (distance == 0);

    return distance; // Return the valid distance
}
