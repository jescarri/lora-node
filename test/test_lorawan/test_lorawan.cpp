#include <unity.h>

#include "lorawan.hpp"
#include "lorawan_settings.hpp"

// Pull in the implementation files so that the code under test is linked into
// the native test executable.  Each PlatformIO test group is built in
// isolation, therefore including the .cpp files directly is the simplest way
// to exercise the production sources without modifying the library list.
#include "../../src/lorawan_settings.cpp"
#include "../../src/lorawan.cpp"

// ---------------------------------------------------------------------------
//  Helper utilities
// ---------------------------------------------------------------------------

static void clear_preferences() {
    lorawan_preferences_init();
}

// Converts a byte buffer to an uppercase hex string (without delimiters) so
// that we can easily compare results against the original configuration
// strings used by the production helpers.
static std::string to_hex(const uint8_t *buf, size_t len) {
    static const char *hex = "0123456789ABCDEF";
    std::string out;
    out.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        out.push_back(hex[(buf[i] >> 4) & 0xF]);
        out.push_back(hex[buf[i] & 0xF]);
    }
    return out;
}

// ---------------------------------------------------------------------------
//  Test cases – os_getArtEui / os_getDevEui / os_getDevKey helpers
// ---------------------------------------------------------------------------

void test_os_getArtEui_valid() {
    clear_preferences();

    const char *value = "0102030405060708"; // 8 bytes → 16 hex digits
    settings_put_string("app_eui", value);

    uint8_t buf[8]{};
    os_getArtEui(buf);

    TEST_ASSERT_EQUAL_STRING(value, to_hex(buf, 8).c_str());
}

void test_os_getArtEui_invalid_length() {
    clear_preferences();
    settings_put_string("app_eui", "ABCDEF"); // too short

    uint8_t buf[8];
    memset(buf, 0xAA, sizeof(buf));
    os_getArtEui(buf);

    // Expect the helper to zero‐fill the destination on error.
    uint8_t expected[8]{};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(buf));
}

void test_os_getDevEui_valid() {
    clear_preferences();

    const char *value = "CAFEBABE42424242";
    settings_put_string("dev_eui", value);

    uint8_t buf[8]{};
    os_getDevEui(buf);

    TEST_ASSERT_EQUAL_STRING(value, to_hex(buf, 8).c_str());
}

void test_os_getDevKey_valid() {
    clear_preferences();

    const char *value = "00112233445566778899AABBCCDDEEFF"; // 16 bytes
    settings_put_string("app_key", value);

    uint8_t buf[16]{};
    os_getDevKey(buf);

    TEST_ASSERT_EQUAL_STRING(value, to_hex(buf, 16).c_str());
}

void test_LoraWANPrintLMICOpmode_executes() {
    // We cannot easily capture Serial output in the stub environment, but we
    // can at least ensure that the helper runs without crashing for an
    // arbitrary combination of flags.

    LMIC.opmode = OP_TXDATA | OP_JOINING | OP_TXRXPEND;
    LoraWANPrintLMICOpmode();

    TEST_ASSERT_TRUE(true); // Dummy assertion – reached when no exception.
}

// ---------------------------------------------------------------------------
//  Unity runner
// ---------------------------------------------------------------------------

void setUp() {}
void tearDown() {}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_os_getArtEui_valid);
    RUN_TEST(test_os_getArtEui_invalid_length);
    RUN_TEST(test_os_getDevEui_valid);
    RUN_TEST(test_os_getDevKey_valid);
    RUN_TEST(test_LoraWANPrintLMICOpmode_executes);

    return UNITY_END();
}

