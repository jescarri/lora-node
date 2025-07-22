#ifndef MENU_HPP_
#define MENU_HPP_
#include <Arduino.h>
#include <FastLED.h>
#ifndef NDEBUG
#include <WiFiManager.h>
#endif
#include <lmic.h>

constexpr int CONFIG_TIMEOUT_SECONDS = 120;
constexpr int APP_EUID_LEN           = 17;
constexpr int MAX_INT_STR_LEN        = 10;

extern CRGB leds[1];

void loadSetings();
#ifndef NDEBUG
void saveConfigCallback();
void configModeCallback(WiFiManager *myWiFiManager);
#else
// Stub implementations for release build
inline void saveConfigCallback() { /* WiFiManager disabled in release build */ }
#endif

void initMenu();
#ifndef NDEBUG
void startWebConf();
void cleanupWiFiManagerParameters();
#else
// Stub implementations for release build
inline void startWebConf() { /* WiFiManager disabled in release build */ }
inline void cleanupWiFiManagerParameters() { /* WiFiManager disabled in release build */ }
#endif

#endif
