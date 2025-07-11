#pragma once
#include <stdint.h>
#include <stddef.h>

// Minimal stub types for native build
typedef int ev_t;
typedef struct { int dummy; } osjob_t;
typedef struct { int seqnoUp, globalDutyRate, globalDutyAvail, txend, txChnl, opmode; uint8_t dataLen, dataBeg; } lmic_t;

typedef uint8_t u1_t;
typedef uint32_t u4_t;
typedef uint32_t devaddr_t;

typedef struct { int dummy; } lmic_pinmap;

// Global LMIC instance for tests - declare as extern
extern lmic_t LMIC;

// Remove or comment out duplicate globals to avoid redefinition
// inline lmic_t SETTINGS_LMIC{};
// inline volatile bool enableSleep_ = true;
// inline bool maxLipoFound = false;

// Provide missing constants
#define LMIC_UNUSED_PIN -1

// Provide stub for LoraWANDebug with correct signature
static inline void LoraWANDebug(const lmic_t&) {}

// Provide other minimal stubs as needed
static inline void LMIC_reset() {}
static inline void os_init() {}
static inline void LMIC_selectSubBand(int) {}
static inline void LMIC_setLinkCheckMode(int) {}
static inline void LMIC_shutdown() {}
static inline int ms2osticksRound(int x) { return x; }
static inline int os_queryTimeCriticalJobs(int) { return 0; }
static inline int osticks2ms(int x) { return x; }
static inline int os_getTime() { return 0; }
static inline void randomSeed(int) {}
static inline int millis() { return 0; }
static inline void LMIC_getSessionKeys(u4_t*, devaddr_t*, u1_t*, u1_t*) {}
static inline void LMIC_setTxData2(uint8_t, const uint8_t*, uint8_t, uint8_t) {}

#define OP_NONE       0x00
#define OP_TXRXPEND   0x01
#define OP_SCAN       0x02
#define OP_TRACK      0x04
#define OP_JOINING    0x08
#define OP_TXDATA     0x10
#define OP_POLL       0x20
#define OP_REJOIN     0x40
#define OP_SHUTDOWN   0x80
#define OP_RNDTX      0x100
#define OP_PINGINI    0x200
#define OP_PINGABLE   0x400
#define OP_NEXTCHNL   0x800
#define OP_LINKDEAD   0x1000
#define OP_TESTMODE   0x2000
#define OP_UNJOIN     0x4000
#define TXRX_PORT     0x80

#define ARDUINO_LMIC_VERSION 0x01020304
#define ARDUINO_LMIC_VERSION_GET_MAJOR(v) (((v) >> 24) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_MINOR(v) (((v) >> 16) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_PATCH(v) (((v) >> 8) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_LOCAL(v) ((v)&0xFF)

#define EV_SCAN_TIMEOUT    1
#define EV_BEACON_FOUND    2
#define EV_BEACON_MISSED   3
#define EV_BEACON_TRACKED  4
#define EV_JOINING         5
#define EV_JOINED          6
#define EV_TXCOMPLETE      7
#define EV_JOIN_FAILED     8
#define EV_REJOIN_FAILED   9
#define EV_LOST_TSYNC      10
#define EV_RESET           11
#define EV_RXCOMPLETE      12
#define EV_LINK_DEAD       13
#define EV_LINK_ALIVE      14
#define EV_TXSTART         15
#define EV_TXCANCELED      16
#define EV_RXSTART         17
#define EV_JOIN_TXCOMPLETE 18
