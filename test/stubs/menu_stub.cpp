#include "Arduino.h"
#include "WiFi.h"
#include "lorawan_settings.hpp"

// Global WiFi instance
WiFiClass WiFi;

// Global Serial instance
SerialClass Serial;

// Global ESP instance
ESPClass ESP;

// Global variables for testing
char wifi_ssid_str[32] = "";
char wifi_password_str[64] = "";

// Minimal menu stub functions
void loadSetings() {}
void initMenu() {}
void saveConfigCallback() {}
void settings_clear() {}

// Arduino function stubs
void delay(unsigned long ms) {}
unsigned long millis() { return 0; }
void digitalWrite(int pin, int value) {}
int digitalRead(int pin) { return 0; }
void pinMode(int pin, int mode) {}
int analogRead(int pin) { return 0; }

// Map function stubs
int map(int value, int fromLow, int fromHigh, int toLow, int toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

float map(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
} 