#include "menu.hpp"
#include "lorawan_settings.hpp"
#include "ota.hpp"
#include <cstring>
#include "utils.hpp"
#include <memory>
#include <array>

#ifdef UNIT_TEST
#include <array>
std::array<char, MAX_LORAWAN_CONF_CHAR_LEN> char_ttn_app_eui;
std::array<char, MAX_LORAWAN_CONF_CHAR_LEN> char_ttn_dev_eui;
std::array<char, MAX_LORAWAN_CONF_CHAR_LEN> char_ttn_app_key;
std::array<char, MAX_INT_STR_LEN> calibration_air_value_str;
std::array<char, MAX_INT_STR_LEN> calibration_water_value_str;
std::array<char, MAX_INT_STR_LEN> sleep_time_hours_str;
#else
char char_ttn_app_eui[MAX_LORAWAN_CONF_CHAR_LEN];
char char_ttn_dev_eui[MAX_LORAWAN_CONF_CHAR_LEN];
char char_ttn_app_key[MAX_LORAWAN_CONF_CHAR_LEN];
char calibration_air_value_str[MAX_INT_STR_LEN];
char calibration_water_value_str[MAX_INT_STR_LEN];
char sleep_time_hours_str[MAX_INT_STR_LEN];
char wifi_ssid_str[32];
char wifi_password_str[64];
#endif

const char* menu[] = {"param", "restart"};

WiFiManager wifiManager;
// Use raw pointers for WiFiManager parameters since WiFiManager doesn't take ownership
// These will be managed manually and cleaned up when no longer needed
WiFiManagerParameter* ttn_app_eui = nullptr;
WiFiManagerParameter* ttn_dev_eui = nullptr;
WiFiManagerParameter* ttn_app_key = nullptr;

WiFiManagerParameter* calibration_air_value   = nullptr;
WiFiManagerParameter* calibration_water_value = nullptr;
WiFiManagerParameter* sleep_time_hours        = nullptr;
WiFiManagerParameter* wifi_ssid               = nullptr;
WiFiManagerParameter* wifi_password           = nullptr;

void loadSetings() {
    if (settings_has_key("app_eui")) {
#ifdef UNIT_TEST
        safe_strncpy(char_ttn_app_eui, settings_get_string("app_eui").c_str());
        Serial.println(char_ttn_app_eui.data());
#else
        safe_strncpy(char_ttn_app_eui, settings_get_string("app_eui").c_str());
        Serial.println(char_ttn_app_eui);
#endif
    } else {
#ifdef UNIT_TEST
        safe_strncpy(char_ttn_app_eui, "00000000");
#else
        safe_strncpy(char_ttn_app_eui, "00000000");
#endif
    }

    if (settings_has_key("dev_eui")) {
#ifdef UNIT_TEST
        safe_strncpy(char_ttn_dev_eui, settings_get_string("dev_eui").c_str());
#else
        safe_strncpy(char_ttn_dev_eui, settings_get_string("dev_eui").c_str());
#endif
    } else {
#ifdef UNIT_TEST
        safe_strncpy(char_ttn_dev_eui, "00000000");
#else
        safe_strncpy(char_ttn_dev_eui, "00000000");
#endif
    }

    if (settings_has_key("app_key")) {
#ifdef UNIT_TEST
        safe_strncpy(char_ttn_app_key, settings_get_string("app_key").c_str());
#else
        safe_strncpy(char_ttn_app_key, settings_get_string("app_key").c_str());
#endif
    } else {
#ifdef UNIT_TEST
        safe_strncpy(char_ttn_app_key, "00000000000000000000000000000000");
#else
        safe_strncpy(char_ttn_app_key, "00000000000000000000000000000000");
#endif
    }
    if (settings_has_key("c_air_v")) {
#ifdef UNIT_TEST
        safe_strncpy(calibration_air_value_str, settings_get_string("c_air_v").c_str());
#else
        safe_strncpy(calibration_air_value_str, settings_get_string("c_air_v").c_str());
#endif
    } else {
#ifdef UNIT_TEST
        safe_strncpy(calibration_air_value_str, "0");
#else
        safe_strncpy(calibration_air_value_str, "0");
#endif
    }
    if (settings_has_key("c_water_v")) {
#ifdef UNIT_TEST
        safe_strncpy(calibration_water_value_str, settings_get_string("c_water_v").c_str());
#else
        safe_strncpy(calibration_water_value_str, settings_get_string("c_water_v").c_str());
#endif
    } else {
#ifdef UNIT_TEST
        safe_strncpy(calibration_water_value_str, "0");
#else
        safe_strncpy(calibration_water_value_str, "0");
#endif
    }
    if (settings_has_key("sleep_hours")) {
#ifdef UNIT_TEST
        safe_strncpy(sleep_time_hours_str, settings_get_string("sleep_hours").c_str());
#else
        safe_strncpy(sleep_time_hours_str, settings_get_string("sleep_hours").c_str());
#endif
    } else {
#ifdef UNIT_TEST
        safe_strncpy(sleep_time_hours_str, "0");
#else
        safe_strncpy(sleep_time_hours_str, "0");
#endif
    }

    // Load WiFi settings
    if (settings_has_key("wifi_ssid")) {
#ifdef UNIT_TEST
        safe_strncpy(wifi_ssid_str, settings_get_string("wifi_ssid").c_str());
#else
        safe_strncpy(wifi_ssid_str, settings_get_string("wifi_ssid").c_str());
#endif
    } else {
#ifdef UNIT_TEST
        safe_strncpy(wifi_ssid_str, "");
#else
        safe_strncpy(wifi_ssid_str, "");
#endif
    }

    if (settings_has_key("wifi_password")) {
#ifdef UNIT_TEST
        safe_strncpy(wifi_password_str, settings_get_string("wifi_password").c_str());
#else
        safe_strncpy(wifi_password_str, settings_get_string("wifi_password").c_str());
#endif
    } else {
#ifdef UNIT_TEST
        safe_strncpy(wifi_password_str, "");
#else
        safe_strncpy(wifi_password_str, "");
#endif
    }
}

void initMenu() {
    WiFiClass::mode(WIFI_STA);
    wifiManager.setMinimumSignalQuality(90);
    wifiManager.setRemoveDuplicateAPs(true);
    wifiManager.setSaveParamsCallback(saveConfigCallback);
    loadSetings();
    ttn_app_eui = new WiFiManagerParameter("app_eui", "AppEUI lsb",
#ifdef UNIT_TEST
                                           char_ttn_app_eui.data()
#else
                                           char_ttn_app_eui
#endif
                                               ,
                                           MAX_LORAWAN_CONF_CHAR_LEN);
    ttn_dev_eui = new WiFiManagerParameter("dev_eui", "DevEUI lsb",
#ifdef UNIT_TEST
                                           char_ttn_dev_eui.data()
#else
                                           char_ttn_dev_eui
#endif
                                               ,
                                           MAX_LORAWAN_CONF_CHAR_LEN);
    ttn_app_key = new WiFiManagerParameter("app_key", "APP Key msb",
#ifdef UNIT_TEST
                                           char_ttn_app_key.data()
#else
                                           char_ttn_app_key
#endif
                                               ,
                                           MAX_LORAWAN_CONF_CHAR_LEN);
    calibration_air_value = new WiFiManagerParameter("calibration_air_value", "Calibration Air Value",
#ifdef UNIT_TEST
                                                     calibration_air_value_str.data()
#else
                                                     calibration_air_value_str
#endif
                                                         ,
                                                     MAX_INT_STR_LEN);
    calibration_water_value = new WiFiManagerParameter("calibration_water_value", "Calibration Water Value",
#ifdef UNIT_TEST
                                                       calibration_water_value_str.data()
#else
                                                       calibration_water_value_str
#endif
                                                           ,
                                                       MAX_INT_STR_LEN);
    sleep_time_hours = new WiFiManagerParameter("sleep_time_hours", "Sleep Time in Hours",
#ifdef UNIT_TEST
                                                sleep_time_hours_str.data()
#else
                                                sleep_time_hours_str
#endif
                                                    ,
                                                MAX_INT_STR_LEN);
    wifi_ssid = new WiFiManagerParameter("wifi_ssid", "WiFi SSID",
#ifdef UNIT_TEST
                                         wifi_ssid_str
#else
                                         wifi_ssid_str
#endif
                                         ,
                                         32);
    wifi_password = new WiFiManagerParameter("wifi_password", "WiFi Password",
#ifdef UNIT_TEST
                                             wifi_password_str
#else
                                             wifi_password_str
#endif
                                             ,
                                             64, "type=\"password\"");
    wifiManager.addParameter(ttn_app_eui);
    wifiManager.addParameter(ttn_dev_eui);
    wifiManager.addParameter(ttn_app_key);
    wifiManager.addParameter(calibration_air_value);
    wifiManager.addParameter(calibration_water_value);
    wifiManager.addParameter(sleep_time_hours);
    wifiManager.addParameter(wifi_ssid);
    wifiManager.addParameter(wifi_password);
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
    
    // Get current WiFi settings BEFORE saving them
    String ssid     = wifi_ssid->getValue();
    String password = wifi_password->getValue();
    
    // Get previous WiFi settings for comparison BEFORE saving new ones
    String prev_ssid     = settings_get_string("wifi_ssid", "");
    String prev_password = settings_get_string("wifi_password", "");
    
    // Save all settings
    settings_put_string("app_eui", ttn_app_eui->getValue());
    settings_put_string("dev_eui", ttn_dev_eui->getValue());
    settings_put_string("app_key", ttn_app_key->getValue());

    settings_put_string("c_air_v", calibration_air_value->getValue());
    settings_put_string("c_water_v", calibration_water_value->getValue());
    settings_put_string("sleep_hours", sleep_time_hours->getValue());

    // Save WiFi settings
    settings_put_string("wifi_ssid", ssid.c_str());
    settings_put_string("wifi_password", password.c_str());

    settings_put_bool("ttn_otaa_config", true);

    Serial.print("SSID length: ");
    Serial.println(ssid.length());

    // Check if WiFi should be tested based on preferences and config changes
    bool should_test_wifi = false;
    bool wifi_tested_ok   = settings_get_bool("wifi_tested_ok", false);

    Serial.print("Previous SSID: '");
    Serial.print(prev_ssid);
    Serial.println("'");
    Serial.print("Current SSID: '");
    Serial.print(ssid);
    Serial.println("'");
    Serial.print("WiFi tested OK: ");
    Serial.println(wifi_tested_ok ? "true" : "false");

    // Test WiFi if:
    // 1. wifi_tested_ok is false/nil and SSID is provided
    // 2. WiFi config has changed
    if (ssid.length() > 0) {
        if (!wifi_tested_ok) {
            should_test_wifi = true;
            Serial.println("Testing WiFi: wifi_tested_ok is false");
        } else if (ssid != prev_ssid || password != prev_password) {
            should_test_wifi = true;
            Serial.println("Testing WiFi: config changed");
        } else {
            Serial.println("Skipping WiFi test: config unchanged and previously tested OK");
        }
    } else {
        Serial.println("Skipping WiFi test: no SSID provided");
    }

    if (should_test_wifi) {
        Serial.println("Testing WiFi connection...");
        if (testWifiConnection(ssid, password)) {
            Serial.println("WiFi test successful");
            settings_put_bool("wifi_tested_ok", true);
            leds[0] = CRGB::Green;
        } else {
            Serial.println("WiFi test failed");
            settings_put_bool("wifi_tested_ok", false);
            leds[0] = CRGB::Orange;
        }
    } else {
        leds[0] = CRGB::Green;
    }
    FastLED.show();
    wifiManager.startConfigPortal("lora-node");
}

void configModeCallback(const WiFiManager* /* myWiFiManager */) {
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    Serial.println("[CALLBACK] configModeCallback fired");
}

// Cleanup function to delete allocated WiFiManagerParameter objects
void cleanupWiFiManagerParameters() {
    delete ttn_app_eui;
    delete ttn_dev_eui;
    delete ttn_app_key;
    delete calibration_air_value;
    delete calibration_water_value;
    delete sleep_time_hours;
    delete wifi_ssid;
    delete wifi_password;
    // Reset pointers to nullptr
    ttn_app_eui             = nullptr;
    ttn_dev_eui             = nullptr;
    ttn_app_key             = nullptr;
    calibration_air_value   = nullptr;
    calibration_water_value = nullptr;
    sleep_time_hours        = nullptr;
    wifi_ssid               = nullptr;
    wifi_password           = nullptr;
}
