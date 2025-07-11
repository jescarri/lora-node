#include <unity.h>

#include "menu.hpp"
#include "lorawan_settings.hpp"

// Pull in the implementation of the modules under test so that the functions
// and global variables are linked into this test executable.  PlatformIO
// compiles each test group in isolation and does not automatically build the
// full project sources for the native environment.
#include "../../src/lorawan_settings.cpp"
#include "../../src/menu.cpp"

#ifdef UNIT_TEST
#include "../../test/stubs/esp32_gpio.h"
#include "../../test/stubs/lmic.h"
lmic_t LMIC{};
CRGB leds[1];
volatile bool enableSleep_ = true;
bool maxLipoFound = false;
#endif

// Extern declarations of the internal buffers defined in menu.cpp so that we
// can validate their contents after calling the public functions under test.

extern char char_ttn_app_eui[MAX_LORAWAN_CONF_CHAR_LEN];
extern char char_ttn_dev_eui[MAX_LORAWAN_CONF_CHAR_LEN];
extern char char_ttn_app_key[MAX_LORAWAN_CONF_CHAR_LEN];
extern char calibration_air_value_str[MAX_INT_STR_LEN];
extern char calibration_water_value_str[MAX_INT_STR_LEN];
extern char sleep_time_hours_str[MAX_INT_STR_LEN];

// ---------------------------------------------------------------------------
//  Test helpers
// ---------------------------------------------------------------------------

static void clear_preferences() {
    // Re-initialise the Preferences stub – the lorawan helper resets the
    // namespace on each begin() call which is sufficient for our needs.
    lorawan_preferences_init();
}

// ---------------------------------------------------------------------------
//  Test cases
// ---------------------------------------------------------------------------

void test_load_settings_defaults() {
    clear_preferences();

    loadSetings();

    TEST_ASSERT_EQUAL_STRING("00000000", char_ttn_app_eui);
    TEST_ASSERT_EQUAL_STRING("00000000", char_ttn_dev_eui);
    TEST_ASSERT_EQUAL_STRING("00000000000000000000000000000000", char_ttn_app_key);

    TEST_ASSERT_EQUAL_STRING("0", calibration_air_value_str);
    TEST_ASSERT_EQUAL_STRING("0", calibration_water_value_str);
    TEST_ASSERT_EQUAL_STRING("0", sleep_time_hours_str);
}

void test_load_settings_existing_values() {
    clear_preferences();

    settings_put_string("app_eui", "A1B2C3D4");
    settings_put_string("dev_eui", "D4C3B2A1");
    settings_put_string("app_key", "FEDCBA98765432100123456789ABCDEF");
    settings_put_string("c_air_v", "123");
    settings_put_string("c_water_v", "456");
    settings_put_string("sleep_hours", "8");

    loadSetings();

    TEST_ASSERT_EQUAL_STRING("A1B2C3D4", char_ttn_app_eui);
    TEST_ASSERT_EQUAL_STRING("D4C3B2A1", char_ttn_dev_eui);
    TEST_ASSERT_EQUAL_STRING("FEDCBA98765432100123456789ABCDEF", char_ttn_app_key);

    TEST_ASSERT_EQUAL_STRING("123", calibration_air_value_str);
    TEST_ASSERT_EQUAL_STRING("456", calibration_water_value_str);
    TEST_ASSERT_EQUAL_STRING("8", sleep_time_hours_str);
}

void test_load_settings_buffer_overflow() {
    clear_preferences();
    // Simulate overly long input
    std::string long_val(2 * MAX_LORAWAN_CONF_CHAR_LEN, 'A');
    settings_put_string("app_eui", long_val.c_str());
    loadSetings();
    // Buffer should be null-terminated and not overflow
    TEST_ASSERT_EQUAL_CHAR('\0', char_ttn_app_eui[MAX_LORAWAN_CONF_CHAR_LEN - 1]);
}

// ---------------------------------------------------------------------------
//  Unity runner
// ---------------------------------------------------------------------------

void setUp() {}
void tearDown() {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_load_settings_defaults);
    RUN_TEST(test_load_settings_existing_values);
    RUN_TEST(test_load_settings_buffer_overflow);
    return UNITY_END();
}
