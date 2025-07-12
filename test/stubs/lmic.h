#ifndef LMIC_H
#define LMIC_H

#include <cstdint>

// LMIC type definitions
typedef uint8_t u1_t;
typedef uint32_t u4_t;
typedef uint32_t devaddr_t;

// LMIC pinmap structure
typedef struct {
    int nss;
    int rxtx;
    int rst;
    int dio[3];
    int rxtx_rx_active;
    int spi_freq;
} lmic_pinmap;

// LMIC event types
typedef enum {
    EV_SCAN_TIMEOUT,
    EV_BEACON_FOUND,
    EV_BEACON_MISSED,
    EV_BEACON_TRACKED,
    EV_JOINING,
    EV_JOINED,
    EV_JOIN_FAILED,
    EV_REJOIN_FAILED,
    EV_TXCOMPLETE,
    EV_LOST_TSYNC,
    EV_RESET,
    EV_RXCOMPLETE,
    EV_LINK_DEAD,
    EV_LINK_ALIVE,
    EV_TXSTART,
    EV_TXCANCELED,
    EV_RXSTART,
    EV_JOIN_TXCOMPLETE
} ev_t;

// LMIC operation modes
#define OP_NONE 0x0000
#define OP_SCAN 0x0001
#define OP_TRACK 0x0002
#define OP_JOINING 0x0004
#define OP_TXDATA 0x0008
#define OP_POLL 0x0010
#define OP_REJOIN 0x0020
#define OP_SHUTDOWN 0x0040
#define OP_TXRXPEND 0x0080
#define OP_RNDTX 0x0100
#define OP_PINGINI 0x0200
#define OP_PINGABLE 0x0400
#define OP_NEXTCHNL 0x0800
#define OP_LINKDEAD 0x1000
#define OP_TESTMODE 0x2000
#define OP_UNJOIN 0x4000

// TXRX flags
#define TXRX_PORT 0x01

// LMIC structure
typedef struct { 
    int seqnoUp, globalDutyRate, globalDutyAvail, txend, txChnl, opmode; 
    uint8_t dataLen, dataBeg;
    uint8_t txrxFlags;
    uint8_t frame[256];  // Buffer for frame data
} lmic_t;

// Global LMIC instance
extern lmic_t LMIC;

// Function declarations
void LMIC_reset();
void LMIC_setLinkCheckMode(int);
void LMIC_selectSubBand(int);
void LMIC_shutdown();
void LMIC_setTxData2(uint8_t port, const uint8_t* data, uint8_t len, uint8_t confirmed);
void os_init();
void os_runloop_once();
bool os_queryTimeCriticalJobs(unsigned long);
unsigned long os_getTime();
unsigned long ms2osticksRound(unsigned long);
unsigned long osticks2ms(unsigned long);
unsigned long LMICbandplan_nextTx(unsigned long);

// Session key functions
void LMIC_getSessionKeys(uint32_t*, uint32_t*, uint8_t*, uint8_t*);

// Job structure
typedef struct {
    void (*func)(void*);
    void* arg;
} osjob_t;

// Function declarations for the code
void os_getArtEui(uint8_t*);
void os_getDevEui(uint8_t*);
void os_getDevKey(uint8_t*);

// Version macros
#define ARDUINO_LMIC_VERSION 0x04010000
#define ARDUINO_LMIC_VERSION_GET_MAJOR(v) (((v) >> 24) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_MINOR(v) (((v) >> 16) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_PATCH(v) (((v) >> 8) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_LOCAL(v) ((v) & 0xFF)

#endif // LMIC_H
