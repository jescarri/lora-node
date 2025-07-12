// Minimal stub for the Arduino WiFi (ESP32) API so that the production code
// can be compiled natively during unit-testing.

#ifndef WIFI_H
#define WIFI_H

#ifdef UNIT_TEST
char wifi_ssid_str[32] = "";
char wifi_password_str[64] = "";
#endif

class WiFiClass {
public:
    static void mode(int) {}
    static void disconnect(bool = true) {}
    static void setTxPower(int) {}
    static int status() { return WL_CONNECTED; }
    static String localIP() { return "192.168.1.100"; }
    static void begin(const char*, const char*) {}
};

#define WIFI_OFF 0
#define WIFI_STA 1
#define WL_CONNECTED 3
#define WIFI_POWER_8_5dBm 0

// Global WiFi instance
extern WiFiClass WiFi;

#endif // WIFI_H

