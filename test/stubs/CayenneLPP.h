#pragma once

class CayenneLPP {
public:
    void reset() {}
    uint8_t getSize() const { return 0; }

    // Add no-op sensor encoding helpers used by production code.
    template <typename... Args>
    void addGenericSensor(Args...) {}

    template <typename... Args>
    void addVoltage(Args...) {}

    template <typename... Args>
    void addPercentage(Args...) {}

    template <typename... Args>
    void addSwitch(Args...) {}

    uint8_t *getBuffer() { return nullptr; }
};

// Global instance expected by lorawan.cpp (declared extern in header).
inline CayenneLPP lpp;
