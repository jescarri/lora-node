#include "Arduino.h"
#include "WiFi.h"
#include "FastLED.h"
#include "lmic.h"
#include "lorawan_settings.hpp"

// OTA structure definition for native testing
struct OtaUpdateInfo {
    String url;
    String md5sum;
    String version;
};

// Global instances
SerialClass Serial;
WiFiClass WiFi;
FastLEDClass FastLED;
ESPClass ESP;

// LMIC global instance
lmic_t LMIC = {};

// Settings global instance
Preferences settings;

// Global lpp instance
CayenneLPP lpp(51);

// WiFi global variables for UNIT_TEST
#ifdef UNIT_TEST
char wifi_ssid_str[32] = "";
char wifi_password_str[64] = "";
#endif

// Arduino function implementations
void delay(unsigned long) {}
unsigned long millis() { return 0; }

void pinMode(int, int) {}
void digitalWrite(int, int) {}
int digitalRead(int) { return 0; }
int analogRead(int) { return 0; }

void randomSeed(unsigned long) {}
int random(int) { return 0; }
int random(int, int) { return 0; }

// LMIC function implementations
void LMIC_reset() {}
void LMIC_setLinkCheckMode(int) {}
void LMIC_selectSubBand(int) {}
void LMIC_shutdown() {}
void LMIC_setTxData2(uint8_t, const uint8_t*, uint8_t, uint8_t) {}
void os_init() {}
void os_runloop_once() {}
bool os_queryTimeCriticalJobs(unsigned long) { return false; }
unsigned long os_getTime() { return 0; }
unsigned long ms2osticksRound(unsigned long x) { return x; }
unsigned long osticks2ms(unsigned long x) { return x; }
unsigned long LMICbandplan_nextTx(unsigned long) { return 0; }
void LMIC_getSessionKeys(uint32_t*, uint32_t*, uint8_t*, uint8_t*) {}
void os_getArtEui(uint8_t*) {}
void os_getDevEui(uint8_t*) {}
void os_getDevKey(uint8_t*) {}

// Settings function implementations
String settings_get_string(const char* key) { return String(""); }
void settings_put_string(const char* key, const String& value) {}
void settings_clear() {}
bool settings_has_key(const char* key) { return false; }
bool settings_get_bool(const char* key, bool defaultValue) { return defaultValue; }

// OTA function implementations (stubs for native testing)
bool verifyMd5Sum(const uint8_t* data, unsigned long len, const String& expectedHash) { return false; }

// Calibration function implementations with return statements
int get_calibration_air_value() { return 0; }
int get_calibration_water_value() { return 0; }

// Missing function implementations
void LoraWANDebug(const lmic_t&) {}
void handleDownlinkMessage(uint8_t*, uint8_t) {}
void reportFirmwareVersion(class CayenneLPP&) {}

// Additional missing functions
void loadSetings() {}
void initMenu() {}
void saveConfigCallback() {} 