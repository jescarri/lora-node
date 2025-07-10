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
} Serial;

#define HIGH 1
#define LOW  0

using byte = uint8_t;

// Bring in additional stubs that many source files expect after the generic
// Arduino header.
#include "WiFi.h"
#include "esp_system.h"
