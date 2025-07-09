#include "menu.hpp"
#include "lorawan_settings.hpp"
#include <cstring>

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
    if (settings_has_key("app_eui")) {
        strncpy(char_ttn_app_eui, settings_get_string("app_eui").c_str(),
                sizeof(char_ttn_app_eui) - 1);
        char_ttn_app_eui[sizeof(char_ttn_app_eui) - 1] = '\0';
        Serial.println(char_ttn_app_eui);
    } else {
        strncpy(char_ttn_app_eui, "00000000", sizeof(char_ttn_app_eui) - 1);
        char_ttn_app_eui[sizeof(char_ttn_app_eui) - 1] = '\0';
    }

    if (settings_has_key("dev_eui")) {
        strncpy(char_ttn_dev_eui, settings_get_string("dev_eui").c_str(),
                sizeof(char_ttn_dev_eui) - 1);
        char_ttn_dev_eui[sizeof(char_ttn_dev_eui) - 1] = '\0';
    } else {
        strncpy(char_ttn_dev_eui, "00000000", sizeof(char_ttn_dev_eui) - 1);
        char_ttn_dev_eui[sizeof(char_ttn_dev_eui) - 1] = '\0';
    }

    if (settings_has_key("app_key")) {
        strncpy(char_ttn_app_key, settings_get_string("app_key").c_str(),
                sizeof(char_ttn_app_key) - 1);
        char_ttn_app_key[sizeof(char_ttn_app_key) - 1] = '\0';
    } else {
        strncpy(char_ttn_app_key,
                "00000000000000000000000000000000",
                sizeof(char_ttn_app_key) - 1);
        char_ttn_app_key[sizeof(char_ttn_app_key) - 1] = '\0';
    }
    if (settings_has_key("c_air_v")) {
        strncpy(calibration_air_value_str,
                settings_get_string("c_air_v").c_str(),
                sizeof(calibration_air_value_str) - 1);
        calibration_air_value_str[sizeof(calibration_air_value_str) - 1] =
            '\0';
    } else {
        strncpy(calibration_air_value_str, "0",
                sizeof(calibration_air_value_str) - 1);
        calibration_air_value_str[sizeof(calibration_air_value_str) - 1] =
            '\0';
    }
    if (settings_has_key("c_water_v")) {
        strncpy(calibration_water_value_str,
                settings_get_string("c_water_v").c_str(),
                sizeof(calibration_water_value_str) - 1);
        calibration_water_value_str[sizeof(calibration_water_value_str) - 1] =
            '\0';
    } else {
        strncpy(calibration_water_value_str, "0",
                sizeof(calibration_water_value_str) - 1);
        calibration_water_value_str[sizeof(calibration_water_value_str) - 1] =
            '\0';
    }
    if (settings_has_key("sleep_hours")) {
        strncpy(sleep_time_hours_str,
                settings_get_string("sleep_hours").c_str(),
                sizeof(sleep_time_hours_str) - 1);
        sleep_time_hours_str[sizeof(sleep_time_hours_str) - 1] = '\0';
    } else {
        strncpy(sleep_time_hours_str, "0", sizeof(sleep_time_hours_str) - 1);
        sleep_time_hours_str[sizeof(sleep_time_hours_str) - 1] = '\0';
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
    settings_put_string("app_eui", ttn_app_eui->getValue());
    settings_put_string("dev_eui", ttn_dev_eui->getValue());
    settings_put_string("app_key", ttn_app_key->getValue());

    settings_put_string("c_air_v", calibration_air_value->getValue());
    settings_put_string("c_water_v", calibration_water_value->getValue());
    settings_put_string("sleep_hours", sleep_time_hours->getValue());

    settings_put_bool("ttn_otaa_config", true);
    leds[0] = CRGB::Green;
    FastLED.show();
}

void configModeCallback(WiFiManager *myWiFiManager) {
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    Serial.println("[CALLBACK] configModeCallback fired");
}
