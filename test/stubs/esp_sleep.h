#pragma once

// Stub for the ESP32 deep-sleep API.  Only the subset used by main.cpp is
// provided and the functions are implemented as empty no-ops so that the code
// can be compiled and linked on the host.

using esp_sleep_pd_domain_t = int;
using esp_sleep_pd_option_t = int;

inline void esp_sleep_pd_config(esp_sleep_pd_domain_t, esp_sleep_pd_option_t) {}
inline void esp_sleep_enable_timer_wakeup(unsigned long long) {}
inline void esp_deep_sleep_start() {}

// Power-domain constants referenced by the production code.  The concrete
// numeric values are irrelevant for the unit tests – they just need to be
// unique.

constexpr esp_sleep_pd_domain_t ESP_PD_DOMAIN_RTC_PERIPH = 0;
constexpr esp_sleep_pd_domain_t ESP_PD_DOMAIN_RTC_SLOW_MEM = 1;
constexpr esp_sleep_pd_domain_t ESP_PD_DOMAIN_RTC_FAST_MEM = 2;
constexpr esp_sleep_pd_domain_t ESP_PD_DOMAIN_XTAL = 3;

constexpr esp_sleep_pd_option_t ESP_PD_OPTION_OFF = 0;

