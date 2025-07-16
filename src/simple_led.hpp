#pragma once

#include <Arduino.h>

class SimpleLED {
private:
    int pin;
    
public:
    SimpleLED(int ledPin) : pin(ledPin) {
        pinMode(pin, OUTPUT);
    }
    
    void setColor(uint8_t r, uint8_t g, uint8_t b) {
        // For a single RGB LED, you'd typically need 3 pins
        // This is a simplified example - adjust based on your LED type
        digitalWrite(pin, (r > 0 || g > 0 || b > 0) ? HIGH : LOW);
    }
    
    void off() {
        digitalWrite(pin, LOW);
    }
    
    void red() { setColor(255, 0, 0); }
    void green() { setColor(0, 255, 0); }
    void orange() { setColor(255, 165, 0); }
};
