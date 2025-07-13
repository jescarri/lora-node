// Minimal stub for the Arduino WiFi (ESP32) API so that the production code
// can be compiled natively during unit-testing.

#ifndef WIFI_H
#define WIFI_H

#include "Arduino.h"

// WiFi status constants
#define WL_CONNECTED 3
#define WL_DISCONNECTED 6
#define WL_NO_SSID_AVAIL 7
#define WL_CONNECT_FAILED 4

#define WIFI_STA 1
#define WIFI_POWER_8_5dBm 34

// Minimal WiFi class
class WiFiClass {
public:
    WiFiClass() = default;
    ~WiFiClass() = default;
    
    bool begin(const char* ssid, const char* password) { return true; }
    bool begin(const char* ssid) { return true; }
    void disconnect() {}
    int status() { return WL_CONNECTED; }
    bool isConnected() { return true; }
    const char* SSID() { return "test_ssid"; }
    int8_t RSSI() { return -50; }
    void mode(int mode) {}
    int getMode() { return 1; }
    void setSleepMode(int sleepMode) {}
    void setTxPower(int power) {}
    
    static WiFiClass& getInstance() {
        static WiFiClass instance;
        return instance;
    }
};

// Global WiFi instance
extern WiFiClass WiFi;

// Minimal WiFiManager class
class WiFiManager {
public:
    WiFiManager() = default;
    ~WiFiManager() = default;
    
    void setAPCallback(void (*func)(WiFiManager*)) {}
    void setSaveConfigCallback(void (*func)()) {}
    void setConfigPortalTimeout(int timeout) {}
    void setConnectTimeout(int timeout) {}
    void setBreakAfterConfig(bool shouldBreak) {}
    void setMinimumSignalQuality(int quality) {}
    void setRemoveDuplicateAPs(bool remove) {}
    void setCountry(const char* country) {}
    void setHostname(const char* hostname) {}
    
    bool startConfigPortal(const char* apName, const char* apPassword = nullptr) { return true; }
    bool autoConnect(const char* apName, const char* apPassword = nullptr) { return true; }
    bool autoConnect() { return true; }
    
    void resetSettings() {}
    void erase() {}
    
    // Parameter management
    class WiFiManagerParameter* addParameter(class WiFiManagerParameter* p) { return p; }
    class WiFiManagerParameter* getParameter(const char* id) { return nullptr; }
    
    // Configuration
    void setConfigPortalBlocking(bool blocking) {}
    void setEnableConfigPortal(bool enable) {}
    void setEnableConnect(bool enable) {}
    void setEnableDebugOutput(bool enable) {}
    
    // Utility
    void process() {}
    bool getConfigPortalActive() { return false; }
    void stopConfigPortal() {}
    
    // Network info
    String getSSID() { return "test_ssid"; }
    String getPassword() { return "test_password"; }
    String getConfigPortalSSID() { return "config_ssid"; }
    String getConfigPortalPassword() { return "config_password"; }
};

// Minimal WiFiManagerParameter class
class WiFiManagerParameter {
public:
    WiFiManagerParameter(const char* custom) : id_(""), placeholder_(""), value_(""), length_(0), custom_(custom) {}
    WiFiManagerParameter(const char* id, const char* placeholder, const char* defaultValue, int length) 
        : id_(id ? id : ""), placeholder_(placeholder ? placeholder : ""), value_(defaultValue ? defaultValue : ""), length_(length), custom_("") {}
    WiFiManagerParameter(const char* id, const char* placeholder, const char* defaultValue, int length, const char* custom) 
        : id_(id ? id : ""), placeholder_(placeholder ? placeholder : ""), value_(defaultValue ? defaultValue : ""), length_(length), custom_(custom ? custom : "") {}
    WiFiManagerParameter(const char* id, const char* placeholder, const char* defaultValue, int length, const char* custom, int labelPlacement) 
        : id_(id ? id : ""), placeholder_(placeholder ? placeholder : ""), value_(defaultValue ? defaultValue : ""), length_(length), custom_(custom ? custom : ""), labelPlacement_(labelPlacement) {}
    
    ~WiFiManagerParameter() = default;
    
    const char* getID() { return id_.c_str(); }
    const char* getValue() { return value_.c_str(); }
    const char* getPlaceholder() { return placeholder_.c_str(); }
    int getValueLength() { return length_; }
    const char* getCustomHTML() { return custom_.c_str(); }
    int getLabelPlacement() { return labelPlacement_; }
    
    void setValue(const char* value, int length) { 
        value_ = value ? value : ""; 
        length_ = length;
    }
    void setValue(const char* value) { value_ = value ? value : ""; }
    void setPlaceholder(const char* placeholder) { placeholder_ = placeholder ? placeholder : ""; }
    void setLabelPlacement(int labelPlacement) { labelPlacement_ = labelPlacement; }
    void setCustomHTML(const char* customHTML) { custom_ = customHTML ? customHTML : ""; }

private:
    std::string id_;
    std::string placeholder_;
    std::string value_;
    int length_;
    std::string custom_;
    int labelPlacement_ = 0;
};

// WiFi modes
#define WIFI_OFF 0
#define WIFI_AP 2
#define WIFI_AP_STA 3

// WiFi sleep modes
#define WIFI_NONE_SLEEP 0
#define WIFI_LIGHT_SLEEP 1
#define WIFI_MODEM_SLEEP 2

// WiFi power levels
#define WIFI_POWER_19_5dBm 78
#define WIFI_POWER_19dBm 76
#define WIFI_POWER_18_5dBm 74
#define WIFI_POWER_17dBm 68
#define WIFI_POWER_15dBm 60
#define WIFI_POWER_13dBm 52
#define WIFI_POWER_11dBm 44
#define WIFI_POWER_8_5dBm 34
#define WIFI_POWER_7dBm 28
#define WIFI_POWER_5dBm 20
#define WIFI_POWER_2dBm 8
#define WIFI_POWER_MINUS_1dBm -4

#endif // WIFI_H

