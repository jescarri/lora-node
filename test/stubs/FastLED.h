#pragma once
#include <cstdint>

struct CRGB {
    uint8_t r{0}, g{0}, b{0};
    CRGB() = default;
    CRGB(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
};

