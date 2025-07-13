#pragma once

// Centralised configuration constants for the project.
// Config stub for native tests
namespace config {
    constexpr int MaxPayloadSize = 51;
    constexpr int MaxSensorRead = 1;
}

// Constants from lorawan.hpp
constexpr int MAX_PAYLOAD_SIZE = config::MaxPayloadSize;
constexpr int MAX_SENSOR_READ = config::MaxSensorRead;

// Constants from main.cpp
constexpr int VCC_ENA_PIN = 13;
constexpr int START_WEB_CONFIG_PIN = 16;
constexpr int NUM_LEDS = 1;
constexpr int LED_DATA_PIN = 17;
constexpr unsigned long long uS_TO_S_FACTOR = 1000000ULL;
const unsigned TX_INTERVAL = 3600; 

// Add externs for WiFi test symbols
#ifdef UNIT_TEST
extern char wifi_ssid_str[32];
extern char wifi_password_str[64];
#endif 