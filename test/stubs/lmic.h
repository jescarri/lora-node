#pragma once
#include <cstdint>

using u1_t = uint8_t;

struct lmic_t {
    uint32_t globalDutyAvail = 0;
    // The real LMIC struct is far larger, but for unit-testing the settings
    // module we only need space to ensure that sizeof(lmic_t) is greater than
    // zero and that the globalDutyAvail field exists.
    uint8_t dummy[16]{};
};

inline lmic_t LMIC{};
inline lmic_t SETTINGS_LMIC{};

struct lmic_pinmap {
    uint8_t nss;
};

struct osjob_t {
    uint8_t dummy;
};

using ev_t = int;

inline void LoraWANDebug(lmic_t) {}
