#pragma once

class CayenneLPP {
public:
    void reset() {}
    uint8_t getSize() const { return 0; }

    // No-op sensor encoding helpers used by the production code.
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

// Global instance expected by lorawan.cpp (declared `extern` in its header).
inline CayenneLPP lpp;
