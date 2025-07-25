#ifndef LORAWAN_SETTINGS_HPP_
#define LORAWAN_SETTINGS_HPP_
#include <FastLED.h>
#include <lmic.h>

#include <hal/hal.h>

constexpr const char *LMIC_PREF_NS_NAME          = "lmic";
constexpr const char *LMIC_INIT_NEEDED_KEY_NAME  = "init";
constexpr const char *LMIC_BYTES_KEY_NAME        = "lmic_struct";
constexpr const char *LORAWAN_CONFIG_PRESENT_KEY = "lorawan_config";
constexpr const char *APP_EUID_KEY               = "euid";
constexpr const char *RUNTIME_KEY                = "runtime";
constexpr int MAX_LORAWAN_CONF_CHAR_LEN          = 100;

constexpr bool RW_MODE = false;
constexpr bool RO_MODE = true;

extern lmic_t SETTINGS_LMIC;
extern lmic_t LMIC;
extern CRGB leds[1];

void lorawan_preferences_init();
void lmic_save();
void save_runtime(long runtime);
long get_previous_runtime();
bool lmic_init_needed();
void load_lmic();
void resetLmic();
bool lorawanConfigPresent();
int get_calibration_air_value();
int get_calibration_water_value();
int get_sleep_time_seconds();

// Generic preference helpers – hide direct dependency on Arduino Preferences
bool settings_has_key(const char *key);
String settings_get_string(const char *key, const char *default_value = "");
void settings_put_string(const char *key, const char *value);
bool settings_get_bool(const char *key, bool default_value = false);
void settings_put_bool(const char *key, bool value);
#endif
