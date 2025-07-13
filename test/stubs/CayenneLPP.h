#ifndef CAYENNE_LPP_H
#define CAYENNE_LPP_H

#include <cstdint>

class CayenneLPP {
public:
    void reset() {}
    void addGenericSensor(uint8_t, float) {}
    void addVoltage(uint8_t, float) {}
    void addPercentage(uint8_t, float) {}
    void addSwitch(uint8_t, bool) {}
    uint8_t* getBuffer() { static uint8_t buf[64] = {0}; return buf; }
    uint8_t getSize() { return 0; }
};

extern CayenneLPP lpp;

#endif // CAYENNE_LPP_H
