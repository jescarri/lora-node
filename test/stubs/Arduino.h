#pragma once
#include <string>
#include <cstdint>

// A very small subset of Arduino's String class sufficient for unit testing.
class String : public std::string {
public:
    String() = default;
    String(const char *s) : std::string(s) {}
    String(const std::string &s) : std::string(s) {}

    // Convenience helpers mimicking the behaviour of the Arduino String API
    // that are used by the production code but missing from std::string.
    bool isEmpty() const { return this->empty(); }

    // Return the substring in the half-open interval [from, to).
    String substring(size_t from, size_t to) const {
        if (from >= size() || to <= from) {
            return String();
        }
        return this->substr(from, to - from);
    }

    // Arduino's String exposes c_str() already via std::string.
};

// Minimal Serial mock – discards all output.
struct {
    template <typename T>
    void print(const T &) const {}

    template <typename T>
    void print(const T &, int /*base*/) const {}

    template <typename T>
    void println(const T &) const {}

    template <typename T>
    void println(const T &, int /*base*/) const {}

    // Overload matching Serial.println() with no arguments which just prints
    // a new line in the real implementation. In the stub this is a no-op.
    void println() const {}

    // Format-style output helper used by production code; always returns the
    // number of bytes that would have been written (zero here because we drop
    // the data in the stub environment).
    template <typename... Args>
    int printf(const char *, Args...) const { return 0; }
} Serial;

// ---------------------------------------------------------------------------
// Arduino formatting constants (base indicators) expected by production code.
// ---------------------------------------------------------------------------
#ifndef DEC
#define DEC 10
#endif

#ifndef HEX
#define HEX 16
#endif

#define HIGH 1
#define LOW  0

using byte = uint8_t;

#ifndef F
#define F(x) x
#endif

// Delay stub – no-op in the host environment.
inline void delay(unsigned long) {}

// Simple linear map function replicating Arduino's behaviour but for `long`
// values only (sufficient for the code under test).
template <typename T>
static inline T map(T x, T in_min, T in_max, T out_min, T out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Dedicated overload for floating-point input which ensures that all
// parameters are treated as double to avoid the template type deduction issue
// seen when mixing `float` and integral constants.
inline float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ADC stub
inline int analogRead(int) { return 0; }

// The AVR/ESP `memcpy_P` helper copies from program memory (flash). In the
// host environment we do not distinguish between flash and RAM, so a regular
// `memcpy` is sufficient.
#ifndef memcpy_P
#include <cstring>
#define memcpy_P(dest, src, n) std::memcpy((dest), (src), (n))
#endif


// Bring in additional stubs that many source files expect after the generic
// Arduino header.
#include "WiFi.h"
#include "esp_system.h"
