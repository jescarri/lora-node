#include "lorawan_settings.hpp"
#include "lorawan.hpp"

void lorawan_preferences_init() {
  lorawan_preferences.begin(LMIC_PREF_NS_NAME, RW_MODE);
}

bool lmic_init_needed() {
  return !lorawan_preferences.isKey(LMIC_BYTES_KEY_NAME);
}

void lmic_save() {
  LMIC.globalDutyAvail = 0;
  size_t a =
      lorawan_preferences.putBytes(LMIC_BYTES_KEY_NAME, &LMIC, sizeof(LMIC));
  Serial.print("Saved: ");
  Serial.println(a);
}

void load_lmic() {
  Serial.println("Start of Load LMIC");
  size_t schlen = lorawan_preferences.getBytesLength(LMIC_BYTES_KEY_NAME);
  char buffer[schlen];
  lorawan_preferences.getBytes(LMIC_BYTES_KEY_NAME, buffer, schlen);
  lmic_t *tmp = (lmic_t *)buffer;
  LMIC = *tmp;
  LoraWANDebug(LMIC);
  Serial.println("END of LoadLmic");
}

void resetLmic() {
   if (lorawan_preferences.isKey(LMIC_BYTES_KEY_NAME)){
    lorawan_preferences.remove(LMIC_BYTES_KEY_NAME);
   }
}

bool lorawanConfigPresent() {
  return lorawan_preferences.isKey(LORAWAN_CONFIG_PRESENT_KEY);
}

int get_calibration_air_value() {
  if (lorawan_preferences.isKey("c_air_v")) {
    return atoi(lorawan_preferences.getString("c_air_v").c_str());
  } else {
    return 0;
  }
}
int get_calibration_water_value() {
  if (lorawan_preferences.isKey("c_water_v")) {
    return atoi(lorawan_preferences.getString("c_water_v").c_str());
  } else {
    return 0;
  }
}
int get_sleep_time_seconds() {
  if (lorawan_preferences.isKey("sleep_hours")) {
    int x = atoi(lorawan_preferences.getString("sleep_hours").c_str());
    if (x > 0) {
      return x * 3600;
    }
  }
  return 3600;
}
