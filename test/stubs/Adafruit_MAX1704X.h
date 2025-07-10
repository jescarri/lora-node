#pragma once
#include <cstdint>

class Adafruit_MAX17048 {
public:
    bool begin() { return false; }
    float cellVoltage() { return 0.0f; }
    float batteryPercent() { return 0.0f; }
};

