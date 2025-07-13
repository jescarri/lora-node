#ifndef LORAWAN_SETTINGS_HPP
#define LORAWAN_SETTINGS_HPP

#include "Arduino.h"
#include "lmic.h"

// Minimal lorawan_settings functions
void loadSetings();
void initMenu();
void saveConfigCallback();
void settings_clear();

// Global variables for testing
extern char wifi_ssid_str[32];
extern char wifi_password_str[64];
extern CayenneLPP lpp;

#endif // LORAWAN_SETTINGS_HPP 