#ifndef FASTLED_H
#define FASTLED_H

#include <Arduino.h>

struct CRGB {
    uint8_t r, g, b;
    
    CRGB() : r(0), g(0), b(0) {}
    CRGB(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
    
    // Predefined colors
    static const CRGB Black;
    static const CRGB Red;
    static const CRGB Green;
    static const CRGB Orange;
    static const CRGB DarkOrange;
};

// Define the static members as inline to avoid multiple definition errors
inline const CRGB CRGB::Black(0, 0, 0);
inline const CRGB CRGB::Red(255, 0, 0);
inline const CRGB CRGB::Green(0, 255, 0);
inline const CRGB CRGB::Orange(255, 165, 0);
inline const CRGB CRGB::DarkOrange(255, 140, 0);

class FastLEDClass {
public:
    static void addLeds(int, int, int) {}
    static void setBrightness(int) {}
    static void show() {}
};

#define WS2812B 0
#define GRB 0

extern FastLEDClass FastLED;

#endif // FASTLED_H
