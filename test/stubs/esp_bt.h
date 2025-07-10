#pragma once

// Minimal stub for the ESP32 Bluetooth controller API used by the production
// code.  We only need to provide the functions that are referenced, all of
// which can be empty no-ops in the host environment.

inline void esp_bt_controller_disable() {}

