#include "lorawan_settings.hpp"
#include "lorawan.hpp"
#include <Preferences.h>

/*
 * Keep the Arduino Preferences instance private to this translation unit so
 * that the rest of the code base no longer depends on <Preferences.h> nor on
 * the concrete Preferences type.  Access to the NVS storage is provided via
 * the small Settings helper functions defined below.
 */

namespace {
//  Singleton accessor – ensures the Preferences instance is created on first
//  use and avoids a non-const global variable that triggers static-analysis
//  warnings.  Because the object lives in an anonymous namespace it is not
//  visible from other translation units.
Preferences &prefs() {
    static Preferences instance;
    return instance;
}
} // namespace

void lorawan_preferences_init() {
    prefs().begin(LMIC_PREF_NS_NAME, RW_MODE);
}

bool lmic_init_needed() {
    return !prefs().isKey(LMIC_BYTES_KEY_NAME);
}

void lmic_save() {
    LMIC.globalDutyAvail = 0;
    size_t a = prefs().putBytes(LMIC_BYTES_KEY_NAME, &LMIC, sizeof(LMIC));
    Serial.print("Saved: ");
    Serial.println(a);
}

void load_lmic() {
    Serial.println("Start of Load LMIC");
    size_t schlen = prefs().getBytesLength(LMIC_BYTES_KEY_NAME);
    char buffer[schlen];
    prefs().getBytes(LMIC_BYTES_KEY_NAME, buffer, schlen);
    lmic_t *tmp = (lmic_t *)buffer;
    LMIC        = *tmp;
    LoraWANDebug(LMIC);
    Serial.println("END of LoadLmic");
}

void save_runtime(long runtime) {
    prefs().putLong(RUNTIME_KEY, runtime);
}

long get_previous_runtime() {
    if (prefs().isKey(LMIC_BYTES_KEY_NAME) != 0) {
        return prefs().getLong(RUNTIME_KEY);
    }
    return 0;
}

void resetLmic() {
    if (prefs().isKey(LMIC_BYTES_KEY_NAME)) {
        prefs().remove(LMIC_BYTES_KEY_NAME);
    }
}

bool lorawanConfigPresent() {
    return prefs().isKey(LORAWAN_CONFIG_PRESENT_KEY);
}

int get_calibration_air_value() {
    if (prefs().isKey("c_air_v")) {
        return atoi(prefs().getString("c_air_v").c_str());
    } else {
        return 0;
    }
}
int get_calibration_water_value() {
    if (prefs().isKey("c_water_v")) {
        return atoi(prefs().getString("c_water_v").c_str());
    } else {
        return 0;
    }
}
int get_sleep_time_seconds() {
    if (prefs().isKey("sleep_hours")) {
        int x = atoi(prefs().getString("sleep_hours").c_str());
        if (x > 0) {
            return x * 3600;
        }
    }
    return 3600;
}

// ---------------------------------------------------------------------------
//  Generic helper layer – hides the concrete Preferences implementation from
//  the rest of the code base.
// ---------------------------------------------------------------------------

bool settings_has_key(const char *key) {
    return prefs().isKey(key);
}

String settings_get_string(const char *key, const char *default_value) {
    if (prefs().isKey(key)) {
        return prefs().getString(key);
    }
    return String(default_value);
}

void settings_put_string(const char *key, const char *value) {
    prefs().putString(key, value);
}

bool settings_get_bool(const char *key, bool default_value) {
    if (prefs().isKey(key)) {
        return prefs().getBool(key);
    }
    return default_value;
}

void settings_put_bool(const char *key, bool value) {
    prefs().putBool(key, value);
}
