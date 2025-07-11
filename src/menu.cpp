#include "menu.hpp"
#include "lorawan_settings.hpp"
#include <cstring>
#include "utils.hpp"
#include <memory>

char char_ttn_app_eui[MAX_LORAWAN_CONF_CHAR_LEN];
char char_ttn_dev_eui[MAX_LORAWAN_CONF_CHAR_LEN];
char char_ttn_app_key[MAX_LORAWAN_CONF_CHAR_LEN];
char calibration_air_value_str[MAX_INT_STR_LEN];
char calibration_water_value_str[MAX_INT_STR_LEN];
char sleep_time_hours_str[MAX_INT_STR_LEN];
const char* menu[] = {"param", "restart"};

WiFiManager wifiManager;
std::unique_ptr<WiFiManagerParameter> ttn_app_eui;
std::unique_ptr<WiFiManagerParameter> ttn_dev_eui;
std::unique_ptr<WiFiManagerParameter> ttn_app_key;

std::unique_ptr<WiFiManagerParameter> calibration_air_value;
std::unique_ptr<WiFiManagerParameter> calibration_water_value;
std::unique_ptr<WiFiManagerParameter> sleep_time_hours;

void loadSetings() {
    if (settings_has_key("app_eui")) {
        safe_strncpy(char_ttn_app_eui, settings_get_string("app_eui").c_str());
        Serial.println(char_ttn_app_eui);
    } else {
        safe_strncpy(char_ttn_app_eui, "00000000");
    }

    if (settings_has_key("dev_eui")) {
        safe_strncpy(char_ttn_dev_eui, settings_get_string("dev_eui").c_str());
    } else {
        safe_strncpy(char_ttn_dev_eui, "00000000");
    }

    if (settings_has_key("app_key")) {
        safe_strncpy(char_ttn_app_key, settings_get_string("app_key").c_str());
    } else {
        safe_strncpy(char_ttn_app_key,
                "00000000000000000000000000000000");
    }
    if (settings_has_key("c_air_v")) {
        safe_strncpy(calibration_air_value_str,
                settings_get_string("c_air_v").c_str());
    } else {
        safe_strncpy(calibration_air_value_str, "0");
    }
    if (settings_has_key("c_water_v")) {
        safe_strncpy(calibration_water_value_str,
                settings_get_string("c_water_v").c_str());
    } else {
        safe_strncpy(calibration_water_value_str, "0");
    }
    if (settings_has_key("sleep_hours")) {
        safe_strncpy(sleep_time_hours_str,
                settings_get_string("sleep_hours").c_str());
    } else {
        safe_strncpy(sleep_time_hours_str, "0");
    }
}

void initMenu() {
    WiFi.mode(WIFI_STA);
    wifiManager.setMinimumSignalQuality(90);
    wifiManager.setRemoveDuplicateAPs(true);
    wifiManager.setSaveParamsCallback(saveConfigCallback);
    loadSetings();
    ttn_app_eui = std::unique_ptr<WiFiManagerParameter>(
        new WiFiManagerParameter("app_eui", "AppEUI lsb", char_ttn_app_eui, MAX_LORAWAN_CONF_CHAR_LEN));
    ttn_dev_eui = std::unique_ptr<WiFiManagerParameter>(
        new WiFiManagerParameter("dev_eui", "DevEUI lsb", char_ttn_dev_eui, MAX_LORAWAN_CONF_CHAR_LEN));
    ttn_app_key = std::unique_ptr<WiFiManagerParameter>(
        new WiFiManagerParameter("app_key", "APP Key msb", char_ttn_app_key, MAX_LORAWAN_CONF_CHAR_LEN));
    calibration_air_value = std::unique_ptr<WiFiManagerParameter>(
        new WiFiManagerParameter("calibration_air_value", "Calibration Air Value",
                                 calibration_air_value_str, MAX_INT_STR_LEN));

    calibration_water_value = std::unique_ptr<WiFiManagerParameter>(
        new WiFiManagerParameter("calibration_water_value", "Calibration Water Value",
        calibration_water_value_str, MAX_INT_STR_LEN));

    sleep_time_hours = std::unique_ptr<WiFiManagerParameter>(
        new WiFiManagerParameter("sleep_time_hours", "Sleep Time in Hours",
                                 sleep_time_hours_str, MAX_INT_STR_LEN));

    wifiManager.addParameter(ttn_app_eui.get());
    wifiManager.addParameter(ttn_dev_eui.get());
    wifiManager.addParameter(ttn_app_key.get());
    wifiManager.addParameter(calibration_air_value.get());
    wifiManager.addParameter(calibration_water_value.get());
    wifiManager.addParameter(sleep_time_hours.get());
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

void configModeCallback(const WiFiManager* /* myWiFiManager */) {
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    Serial.println("[CALLBACK] configModeCallback fired");
}
