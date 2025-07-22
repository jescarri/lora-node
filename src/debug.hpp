#pragma once

#include "config.hpp"

// Debug macros are now compile-time disabled for optimized builds
// These macros compile to nothing, ensuring zero runtime overhead
#if 0        // Always disabled for production builds
#define DEBUG_PRINT(x)         Serial.print(x)
#define DEBUG_PRINTLN(x)       Serial.println(x)
#define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, __VA_ARGS__)
#define DEBUG_BEGIN(baud)      Serial.begin(baud)
#else
#define DEBUG_PRINT(x) \
    do {               \
    } while (0)
#define DEBUG_PRINTLN(x) \
    do {                 \
    } while (0)
#define DEBUG_PRINTF(fmt, ...) \
    do {                       \
    } while (0)
#define DEBUG_BEGIN(baud) \
    do {                  \
    } while (0)
#endif
