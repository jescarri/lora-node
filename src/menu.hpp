#ifndef MENU_HPP_
#define MENU_HPP_
#include <Arduino.h>
#include <FastLED.h>
#include <WiFiManager.h>
#include <lmic.h>

#define CONFIG_TIMEOUT_SECONDS 120
#define APP_EUID_LEN           17
#define MAX_INT_STR_LEN        10

extern CRGB leds[1];

void loadSetings();
void saveConfigCallback();
void configModeCallback(WiFiManager *myWiFiManager);

void initMenu();
void startWebConf();

#endif
