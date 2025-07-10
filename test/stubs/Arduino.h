#pragma once
#include <string>
#include <cstdint>

// A very small subset of Arduino's String class sufficient for unit testing.
class String : public std::string {
public:
    String() = default;
    String(const char *s) : std::string(s) {}
    String(const std::string &s) : std::string(s) {}

    // Arduino's String exposes c_str() already via std::string.
};

// Minimal Serial mock – discards all output.
struct {
    template <typename T>
    void print(const T &) const {}

    template <typename T>
    void println(const T &) const {}

    template <typename... Args>
    int printf(const char *, Args...) const { return 0; }
} Serial;

#define HIGH 1
#define LOW  0

using byte = uint8_t;

#ifndef F
#define F(x) x
#endif

// Delay stub – no-op in host environment
inline void delay(unsigned long) {}

// Simple linear map function replicating Arduino's behaviour but for `long`
// values only (sufficient for the code under test).
template <typename T>
static inline T map(T x, T in_min, T in_max, T out_min, T out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ADC stub
inline int analogRead(int) { return 0; }


// Bring in additional stubs that many source files expect after the generic
// Arduino header.
#include "WiFi.h"
#include "esp_system.h"
