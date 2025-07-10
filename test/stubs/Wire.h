#pragma once

// Very small stub for the Arduino Wire (I2C) library sufficient for the unit
// tests.  Provides only the members accessed by src/main.cpp.

class TwoWire {
public:
    void begin(int /*sda*/, int /*scl*/) {}
    void end() {}
};

// Global instance mimicking the AVR `Wire` object.
inline TwoWire Wire;

