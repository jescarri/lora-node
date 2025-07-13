#include "lmic.h"

extern "C" {
    unsigned long os_getTime() { return 0; }
    void LMIC_setLinkCheckMode(int) {}
    void LMIC_getSessionKeys(unsigned int*, unsigned int*, unsigned char*, unsigned char*) {}
    void LMIC_setTxData2(unsigned char, const unsigned char*, unsigned char, unsigned char) {}
    void handleDownlinkMessage(unsigned char*, unsigned char) {}
    void reportFirmwareVersion(void*) {}
    void LoraWANDebug(const lmic_t&) {}
} 