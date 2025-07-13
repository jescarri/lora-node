#ifndef LMIC_H
#define LMIC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char u1_t;
typedef unsigned int  u4_t;
typedef unsigned int  devaddr_t;
typedef struct { int dummy; } osjob_t;
typedef struct {
    int seqnoUp, globalDutyRate, globalDutyAvail, txend, txChnl, opmode;
    uint8_t dataLen, dataBeg;
    uint8_t txrxFlags;
    uint8_t frame[256];
} lmic_t;

typedef struct {
    int nss;
    int rxtx;
    int rst;
    int dio[3];
    int rxtx_rx_active;
    int spi_freq;
} lmic_pinmap;

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

#define OP_NONE      0x0000
#define OP_SCAN      0x0001
#define OP_TRACK     0x0002
#define OP_JOINING   0x0004
#define OP_TXDATA    0x0008
#define OP_POLL      0x0010
#define OP_REJOIN    0x0020
#define OP_SHUTDOWN  0x0040
#define OP_TXRXPEND  0x0080
#define OP_RNDTX     0x0100
#define OP_PINGINI   0x0200
#define OP_PINGABLE  0x0400
#define OP_NEXTCHNL  0x0800
#define OP_LINKDEAD  0x1000
#define OP_TESTMODE  0x2000
#define OP_UNJOIN    0x4000

#define ARDUINO_LMIC_VERSION 0x04010000
#define ARDUINO_LMIC_VERSION_GET_MAJOR(v) (((v) >> 24) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_MINOR(v) (((v) >> 16) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_PATCH(v) (((v) >> 8) & 0xFF)
#define ARDUINO_LMIC_VERSION_GET_LOCAL(v) ((v) & 0xFF)

#define TXRX_PORT 0x01

#ifdef __cplusplus
}
// Inline C++ stubs for test linkage
inline unsigned long os_getTime() { return 0; }
inline void LMIC_setLinkCheckMode(int) {}
inline void LMIC_getSessionKeys(unsigned int*, unsigned int*, unsigned char*, unsigned char*) {}
inline void LMIC_setTxData2(unsigned char, const unsigned char*, unsigned char, unsigned char) {}
inline void handleDownlinkMessage(unsigned char*, unsigned char) {}
inline void reportFirmwareVersion(void*) {}
inline void LoraWANDebug(const lmic_t&) {}
#endif

#endif // LMIC_H
