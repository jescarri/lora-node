#include "menu.hpp"
#include "lorawan_settings.hpp"

char char_ttn_app_eui[MAX_LORAWAN_CONF_CHAR_LEN];
char char_ttn_dev_eui[MAX_LORAWAN_CONF_CHAR_LEN];
char char_ttn_app_key[MAX_LORAWAN_CONF_CHAR_LEN];
char calibration_air_value_str[MAX_INT_STR_LEN];
char calibration_water_value_str[MAX_INT_STR_LEN];
char sleep_time_hours_str[MAX_INT_STR_LEN];
const char *menu[] = {"param", "restart"};

WiFiManager wifiManager;
WiFiManagerParameter *ttn_app_eui;
WiFiManagerParameter *ttn_dev_eui;
WiFiManagerParameter *ttn_app_key;

WiFiManagerParameter *calibration_air_value;
WiFiManagerParameter *calibration_water_value;
WiFiManagerParameter *sleep_time_hours;

void loadSetings() {
    if (lorawan_preferences.isKey("app_eui")) {
        strcpy(char_ttn_app_eui, lorawan_preferences.getString("app_eui").c_str());
        Serial.println(char_ttn_app_eui);
    } else {
        strcpy(char_ttn_app_eui, "00000000");
    }

    if (lorawan_preferences.isKey("dev_eui")) {
        strcpy(char_ttn_dev_eui, lorawan_preferences.getString("dev_eui").c_str());
    } else {
        strcpy(char_ttn_dev_eui, "00000000");
    }

    if (lorawan_preferences.isKey("app_key")) {
        strcpy(char_ttn_app_key, lorawan_preferences.getString("app_key").c_str());
    } else {
        strcpy(char_ttn_app_key, "00000000000000000000000000000000");
    }
    if (lorawan_preferences.isKey("c_air_v")) {
        strcpy(calibration_air_value_str,
               lorawan_preferences.getString("c_air_v").c_str());
    } else {
        strcpy(calibration_air_value_str, "0");
    }
    if (lorawan_preferences.isKey("c_water_v")) {
        strcpy(calibration_water_value_str,
               lorawan_preferences.getString("c_water_v").c_str());
    } else {
        strcpy(calibration_water_value_str, "0");
    }
    if (lorawan_preferences.isKey("sleep_hours")) {
        strcpy(sleep_time_hours_str,
               lorawan_preferences.getString("sleep_hours").c_str());
    } else {
        strcpy(sleep_time_hours_str, "0");
    }
}

void initMenu() {
    WiFi.mode(WIFI_STA);
    wifiManager.setMinimumSignalQuality(90);
    wifiManager.setRemoveDuplicateAPs(true);
    wifiManager.setSaveParamsCallback(saveConfigCallback);
    loadSetings();
    ttn_app_eui = new WiFiManagerParameter(
        "app_eui", "AppEUI lsb", char_ttn_app_eui, MAX_LORAWAN_CONF_CHAR_LEN);
    ttn_dev_eui = new WiFiManagerParameter(
        "dev_eui", "DevEUI lsb", char_ttn_dev_eui, MAX_LORAWAN_CONF_CHAR_LEN);
    ttn_app_key = new WiFiManagerParameter(
        "app_key", "APP Key msb", char_ttn_app_key, MAX_LORAWAN_CONF_CHAR_LEN);
    calibration_air_value =
        new WiFiManagerParameter("calibration_air_value", "Calibration Air Value",
                                 calibration_air_value_str, MAX_INT_STR_LEN);

    calibration_water_value = new WiFiManagerParameter(
        "calibration_water_value", "Calibration Water Value",
        calibration_water_value_str, MAX_INT_STR_LEN);

    sleep_time_hours =
        new WiFiManagerParameter("sleep_time_hours", "Sleep Time in Hours",
                                 sleep_time_hours_str, MAX_INT_STR_LEN);

    wifiManager.addParameter(ttn_app_eui);
    wifiManager.addParameter(ttn_dev_eui);
    wifiManager.addParameter(ttn_app_key);
    wifiManager.addParameter(calibration_air_value);
    wifiManager.addParameter(calibration_water_value);
    wifiManager.addParameter(sleep_time_hours);
    wifiManager.setMenu(menu, sizeof(menu) / sizeof(menu[0]));
}

void startWebConf() {
    leds[0] = CRGB::Red;
    FastLED.show();

    initMenu();
    Serial.println("AfterInitMenu");
    wifiManager.setConfigPortalTimeout(300);
    if (!wifiManager.startConfigPortal("lora-node")) {
        Serial.println("failed to connect and hit timeout");
        ESP.restart();
    }
}

void saveConfigCallback() {
    Serial.println("Should save config");
    lorawan_preferences.putString("app_eui", ttn_app_eui->getValue());
    lorawan_preferences.putString("dev_eui", ttn_dev_eui->getValue());
    lorawan_preferences.putString("app_key", ttn_app_key->getValue());

    lorawan_preferences.putString("c_air_v", calibration_air_value->getValue());
    lorawan_preferences.putString("c_water_v",
                                  calibration_water_value->getValue());
    lorawan_preferences.putString("sleep_hours", sleep_time_hours->getValue());

    lorawan_preferences.putBool("ttn_otaa_config", true);
    leds[0] = CRGB::Green;
    FastLED.show();
}

void configModeCallback(WiFiManager *myWiFiManager) {
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    Serial.println("[CALLBACK] configModeCallback fired");
}
