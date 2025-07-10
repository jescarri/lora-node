#pragma once

// Minimal stub for the ESP-IDF ADC driver header used in the production code.
// Only the symbols required by src/main.cpp are provided.  They do not perform
// any real hardware interaction but allow the code to compile in the native
// host environment.

inline void adc_power_acquire() {}
inline void adc_power_release() {}

