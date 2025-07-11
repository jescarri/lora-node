#include <unity.h>
#include "../stubs/Arduino.h"
#include "../stubs/config.hpp"
#include "lorawan_settings.hpp"
#include "menu.hpp"

void test_main_constants() {
    // Test that constexpr constants are properly defined
    TEST_ASSERT_EQUAL(13, VCC_ENA_PIN);
    TEST_ASSERT_EQUAL(16, START_WEB_CONFIG_PIN);
    TEST_ASSERT_EQUAL(1, NUM_LEDS);
    TEST_ASSERT_EQUAL(17, LED_DATA_PIN);
    TEST_ASSERT_EQUAL(1000000ULL, uS_TO_S_FACTOR);
    TEST_ASSERT_EQUAL(3600, TX_INTERVAL);
}

void test_lorawan_settings_constants() {
    // Test lorawan_settings.hpp constants
    TEST_ASSERT_EQUAL_STRING("lmic", LMIC_PREF_NS_NAME);
    TEST_ASSERT_EQUAL_STRING("init", LMIC_INIT_NEEDED_KEY_NAME);
    TEST_ASSERT_EQUAL_STRING("lmic_struct", LMIC_BYTES_KEY_NAME);
    TEST_ASSERT_EQUAL_STRING("lorawan_config", LORAWAN_CONFIG_PRESENT_KEY);
    TEST_ASSERT_EQUAL_STRING("euid", APP_EUID_KEY);
    TEST_ASSERT_EQUAL_STRING("runtime", RUNTIME_KEY);
    TEST_ASSERT_EQUAL(100, MAX_LORAWAN_CONF_CHAR_LEN);
    TEST_ASSERT_FALSE(RW_MODE);
    TEST_ASSERT_TRUE(RO_MODE);
}

void test_menu_constants() {
    // Test menu.hpp constants
    TEST_ASSERT_EQUAL(120, CONFIG_TIMEOUT_SECONDS);
    TEST_ASSERT_EQUAL(17, APP_EUID_LEN);
    TEST_ASSERT_EQUAL(10, MAX_INT_STR_LEN);
}

void test_lorawan_constants() {
    // Test lorawan.hpp constants
    TEST_ASSERT_EQUAL(config::MaxPayloadSize, MAX_PAYLOAD_SIZE);
    TEST_ASSERT_EQUAL(config::MaxSensorRead, MAX_SENSOR_READ);
}

void setUp() {}
void tearDown() {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_main_constants);
    RUN_TEST(test_lorawan_settings_constants);
    RUN_TEST(test_menu_constants);
    RUN_TEST(test_lorawan_constants);
    return UNITY_END();
} 