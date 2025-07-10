#pragma once
#include <cstdint>

using u1_t = uint8_t;
using u4_t = uint32_t;
using devaddr_t = uint32_t;

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

#ifndef LORAWAN_CPP_IMPLEMENTATION
static inline void LoraWANDebug(lmic_t) {}
#endif

// ---------------------------------------------------------------------------
//  Additional helpers and stubs required by lorawan.cpp
// ---------------------------------------------------------------------------

// LMIC operation flags
constexpr uint8_t OP_NONE       = 0x00;
constexpr uint8_t OP_TXRXPEND   = 0x01;
constexpr uint8_t OP_SCAN       = 0x02;
constexpr uint8_t OP_TRACK      = 0x04;
constexpr uint8_t OP_JOINING    = 0x08;
constexpr uint8_t OP_TXDATA     = 0x10;
constexpr uint8_t OP_POLL       = 0x20;
constexpr uint8_t OP_REJOIN     = 0x40;
constexpr uint8_t OP_SHUTDOWN   = 0x80;
constexpr uint8_t OP_RNDTX      = 0x100;
constexpr uint8_t OP_PINGINI    = 0x200;
constexpr uint8_t OP_PINGABLE   = 0x400;
constexpr uint8_t OP_NEXTCHNL   = 0x800;
constexpr uint8_t OP_LINKDEAD   = 0x1000;
constexpr uint8_t OP_TESTMODE   = 0x2000;
constexpr uint8_t OP_UNJOIN     = 0x4000;

// TXRX flag indicating that the frame contains a port field.
constexpr uint8_t TXRX_PORT = 0x80;

// Event IDs (subset used by production code)
enum {
    EV_SCAN_TIMEOUT,
    EV_BEACON_FOUND,
    EV_BEACON_MISSED,
    EV_BEACON_TRACKED,
    EV_JOINING,
    EV_JOINED,
    EV_TXCOMPLETE,

    // Additional events referenced by production code but not relevant for
    // the unit-tests. They are assigned arbitrary unique values to avoid
    // clashes.
    EV_JOIN_FAILED,
    EV_REJOIN_FAILED,
    EV_LOST_TSYNC,
    EV_RESET,
    EV_RXCOMPLETE,
    EV_LINK_DEAD,
    EV_LINK_ALIVE,
    EV_TXSTART,
    EV_TXCANCELED,
    EV_RXSTART,
    EV_JOIN_TXCOMPLETE,
};

// Version helpers expected by lorawan.cpp
#define ARDUINO_LMIC_VERSION 0x01020304
#define ARDUINO_LMIC_VERSION_GET_MAJOR(v) (((v) >> 24) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_MINOR(v) (((v) >> 16) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_PATCH(v) (((v) >> 8) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_LOCAL(v) ((v)&0xFF)

inline uint32_t LMICbandplan_nextTx(uint32_t) { return 0; }

inline uint32_t osticks2ms(uint32_t v) { return v; }

inline uint32_t os_getTime() { return 0; }

inline void LMIC_getSessionKeys(uint32_t *, uint32_t *, uint8_t *, uint8_t *) {}

inline void LMIC_setLinkCheckMode(int) {}

inline void LMIC_setTxData2(uint8_t /*port*/, const uint8_t * /*data*/, uint8_t /*len*/, uint8_t /*confirmed*/) {}

// ---------------------------------------------------------------------------
//  Global flags normally provided by main.cpp – defined here so that unit
//  tests linking only against the library still succeed.
// ---------------------------------------------------------------------------

inline bool maxLipoFound               = false;
inline volatile bool enableSleep_      = true;
