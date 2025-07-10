#pragma once
#include <cstdint>

class Adafruit_MAX17048 {
public:
    bool begin() { return false; }
    float cellVoltage() { return 0.0f; }
    float cellPercent() { return 0.0f; }
    float chargeRate() { return 0.0f; }
};
