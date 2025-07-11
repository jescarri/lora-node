#include <unity.h>

// The stubs must be visible *before* we include any project headers that rely
// on Arduino or ESP-specific libraries.
#include "Arduino.h"
#include "Preferences.h"
#include <cstring>

#ifdef UNIT_TEST
#include "../../test/stubs/esp32_gpio.h"
#include "../../test/stubs/lmic.h"
lmic_t LMIC{};
#endif

#include "lorawan_settings.hpp"

// Bring in the implementation so that the functions under test are linked
// into this test executable.  This is required because PlatformIO compiles
// each test case in isolation and does not automatically build the full
// project sources for the native environment.
#include "../../src/lorawan_settings.cpp"

// ---------------------------------------------------------------------------
//  Test fixtures
// ---------------------------------------------------------------------------

void setUp() {
    // Re-initialise the (stub) Preferences storage to get a fresh, empty
    // namespace for every individual test case.
    lorawan_preferences_init();
}

void tearDown() {}

// ---------------------------------------------------------------------------
//  settings_* helper layer
// ---------------------------------------------------------------------------

void test_string_roundtrip() {
    const char *key = "foo";
    const char *value = "bar";

    settings_put_string(key, value);
    TEST_ASSERT_TRUE(settings_has_key(key));

    String result = settings_get_string(key, "");
    TEST_ASSERT_EQUAL_STRING(value, result.c_str());
}

void test_string_default() {
    const char *key = "nonexistent";
    const char *default_value = "fallback";

    String result = settings_get_string(key, default_value);
    TEST_ASSERT_EQUAL_STRING(default_value, result.c_str());
}

void test_bool_roundtrip() {
    const char *key = "flag";

    settings_put_bool(key, true);
    TEST_ASSERT_TRUE(settings_get_bool(key, false));

    settings_put_bool(key, false);
    TEST_ASSERT_FALSE(settings_get_bool(key, true));
}

void test_bool_default() {
    const char *key = "nonexistent_bool";
    TEST_ASSERT_FALSE(settings_get_bool(key, false));
}

// ---------------------------------------------------------------------------
//  High-level convenience helpers
// ---------------------------------------------------------------------------

void test_sleep_time_default() {
    // No configuration stored → default (1 h = 3600 s).
    TEST_ASSERT_EQUAL_INT(3600, get_sleep_time_seconds());
}

void test_sleep_time_custom() {
    settings_put_string("sleep_hours", "3");
    TEST_ASSERT_EQUAL_INT(3 * 3600, get_sleep_time_seconds());
}

void test_calibration_default() {
    TEST_ASSERT_EQUAL_INT(0, get_calibration_air_value());
    TEST_ASSERT_EQUAL_INT(0, get_calibration_water_value());
}

void test_calibration_values() {
    settings_put_string("c_air_v", "123");
    settings_put_string("c_water_v", "456");

    TEST_ASSERT_EQUAL_INT(123, get_calibration_air_value());
    TEST_ASSERT_EQUAL_INT(456, get_calibration_water_value());
}

// ---------------------------------------------------------------------------
//  LMIC persistence helpers
// ---------------------------------------------------------------------------

void test_lmic_persistence() {
    // Initially no LMIC state stored → init needed.
    TEST_ASSERT_TRUE(lmic_init_needed());

    // Save current (dummy) LMIC – should clear the flag.
    lmic_save();
    TEST_ASSERT_FALSE(lmic_init_needed());

    // Removing the key should bring us back to the initial state.
    resetLmic();
    TEST_ASSERT_TRUE(lmic_init_needed());
}

// ---------------------------------------------------------------------------
//  Unity test runner
// ---------------------------------------------------------------------------

int main(int, char **) {
    UNITY_BEGIN();

    RUN_TEST(test_string_roundtrip);
    RUN_TEST(test_string_default);
    RUN_TEST(test_bool_roundtrip);
    RUN_TEST(test_bool_default);

    RUN_TEST(test_sleep_time_default);
    RUN_TEST(test_sleep_time_custom);

    RUN_TEST(test_calibration_default);
    RUN_TEST(test_calibration_values);

    RUN_TEST(test_lmic_persistence);

    return UNITY_END();
}
