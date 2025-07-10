#pragma once
#include <cstdint>

using u1_t = uint8_t;

struct lmic_t {
    uint32_t globalDutyAvail = 0;
    uint32_t globalDutyRate  = 0;
    uint32_t seqnoUp         = 0;
    uint32_t txend           = 0;
    uint8_t txChnl           = 0;
    uint8_t opmode           = 0;

    uint8_t dataLen   = 0;
    uint8_t dataBeg   = 0;
    uint8_t txrxFlags = 0;

    uint8_t frame[64]{};
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

// ---------------------------------------------------------------------------
//  Additional helpers and stubs required by lorawan.cpp
// ---------------------------------------------------------------------------

// LMIC operation flags
constexpr uint8_t OP_NONE       = 0x00;
constexpr uint8_t OP_TXRXPEND   = 0x01;

// TXRX flag indicating that the frame contains a port field.
constexpr uint8_t TXRX_PORT = 0x80;

inline uint32_t LMICbandplan_nextTx(uint32_t) { return 0; }

inline uint32_t osticks2ms(uint32_t v) { return v; }

inline uint32_t os_getTime() { return 0; }

inline void LMIC_getSessionKeys(uint32_t *, uint32_t *, uint8_t *, uint8_t *) {}

inline void LMIC_setLinkCheckMode(int) {}

inline void LMIC_setTxData2(uint8_t /*port*/, const uint8_t * /*data*/, uint8_t /*len*/, uint8_t /*confirmed*/) {}
