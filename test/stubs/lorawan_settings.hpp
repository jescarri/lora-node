#ifndef LORAWAN_SETTINGS_HPP
#define LORAWAN_SETTINGS_HPP

#include <Arduino.h>
#include <Preferences.h>

// Settings functions
String settings_get_string(const char* key);
void settings_put_string(const char* key, const String& value);
void settings_clear();
bool settings_has_key(const char* key);
bool settings_get_bool(const char* key, bool defaultValue = false);

// Global settings instance
extern Preferences settings;

#endif // LORAWAN_SETTINGS_HPP 