#pragma once
#include <cstdint>

struct CRGB {
    uint8_t r{0}, g{0}, b{0};
    constexpr CRGB() = default;
    constexpr CRGB(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
    // Common named colours used in the production code.
    static const CRGB Red;
    static const CRGB Green;
    static const CRGB Black;
    static const CRGB DarkOrange;
};

// ---------------------------------------------------------------------------
//  Very small subset of the FastLED API sufficient for unit testing.
// ---------------------------------------------------------------------------

class FastLEDClass {
public:
    template <typename... Args>
    void addLeds(Args &&... /*args*/) {}

    void show() {}

    void setBrightness(int /*b*/) {}
};

static FastLEDClass FastLED;

// Single LED array placeholder used by the menu module.
inline CRGB leds[1];

// ---------------------------------------------------------------------------
//  Colour constant definitions – must appear *after* the CRGB definition.
// ---------------------------------------------------------------------------

inline const CRGB CRGB::Red{255, 0, 0};
inline const CRGB CRGB::Green{0, 255, 0};
inline const CRGB CRGB::Black{0, 0, 0};
inline constexpr CRGB CRGB::DarkOrange{255, 140, 0};
