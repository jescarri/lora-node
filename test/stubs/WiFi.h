// Minimal stub for the Arduino WiFi (ESP32) API so that the production code
// can be compiled natively during unit-testing.

#pragma once

#include <cstdint>

// WiFi mode constants – only the values are irrelevant for unit-tests, they
// just need to compile.
constexpr int WIFI_OFF = 0;
constexpr int WIFI_STA = 1;

// Mock power constants used in the code base.
constexpr int WIFI_POWER_8_5dBm = 0;

class WiFiClass {
public:
    static void mode(int /*m*/) {}
    void disconnect(bool /*wifioff*/ = true) {}
    void setTxPower(int /*pwr*/) {}
};

// Single global instance mimicking the Arduino API.
static WiFiClass WiFi;

