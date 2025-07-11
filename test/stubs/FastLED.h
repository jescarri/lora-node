#pragma once
#include <stdint.h>

struct CRGB {
    uint8_t r, g, b;
    CRGB() : r(0), g(0), b(0) {}
    static const CRGB Black;
    static const CRGB Red;
    static const CRGB Green;
    static const CRGB DarkOrange;
};

inline const CRGB CRGB::Black = CRGB();
inline const CRGB CRGB::Red = []{ CRGB c; c.r = 255; return c; }();
inline const CRGB CRGB::Green = []{ CRGB c; c.g = 255; return c; }();
inline const CRGB CRGB::DarkOrange = []{ CRGB c; c.r = 255; c.g = 140; return c; }();

class FastLEDClass {
public:
    template<typename... Args>
    void addLeds(Args&&...) {}
    void setBrightness(int) {}
    void show() {}
};

static FastLEDClass FastLED;
// Remove or comment out duplicate global variable
// inline CRGB leds[1];
