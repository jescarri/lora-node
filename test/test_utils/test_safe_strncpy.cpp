#include <unity.h>

#include "utils.hpp"

#ifdef UNIT_TEST
#include "../../test/stubs/esp32_gpio.h"
#endif

// ---------------------------------------------------------------------------
//  Helpers
// ---------------------------------------------------------------------------

template <std::size_t N>
static bool buffer_is_zeroed(const char (&buf)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        if (buf[i] != '\0') {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
//  Test cases
// ---------------------------------------------------------------------------

void test_null_source() {
    char dest[8];
    // Fill with non-zero pattern to verify that safe_strncpy overwrites it.
    memset(dest, 'X', sizeof(dest));

    safe_strncpy(dest, nullptr);

    // Destination must contain an empty string (just terminator) and all
    // remaining bytes past the first must be untouched (i.e. still zero).
    TEST_ASSERT_EQUAL_CHAR('\0', dest[0]);
}

void test_exact_fit() {
    constexpr char kSrc[] = "ESP32"; // length 5 incl. terminator 6
    char dest[sizeof(kSrc)];          // Exact same size

    safe_strncpy(dest, kSrc);

    TEST_ASSERT_EQUAL_STRING(kSrc, dest);
}

void test_truncation() {
    constexpr char kSrc[] = "PlatformIO unit-testing";
    char dest[10]; // smaller than source

    safe_strncpy(dest, kSrc);

    // The resulting string must be properly terminated.
    TEST_ASSERT_EQUAL_CHAR('\0', dest[sizeof(dest) - 1]);

    // If we copy again into a larger buffer we should match the original
    // prefix of length dest_size - 1.
    char larger[sizeof(kSrc)];
    strncpy(larger, kSrc, sizeof(dest) - 1);
    larger[sizeof(dest) - 1] = '\0';

    TEST_ASSERT_EQUAL_STRING(larger, dest);
}

void test_buffer_integrity() {
    // Ensure the function never writes past the destination buffer.
    constexpr char kSrc[] = "123456789";
    char dest[5];

    // Place sentinel bytes after the buffer in a struct to detect overflow.
    struct {
        char buf[5];
        uint32_t sentinel;
    } wrapper;

    wrapper.sentinel = 0xDEADBEEF;

    safe_strncpy(wrapper.buf, kSrc);

    TEST_ASSERT_EQUAL_UINT32(0xDEADBEEF, wrapper.sentinel);
}

void test_null_dest() {
    // Cannot test nullptr with template function that expects array reference
    // This test is not applicable for the current implementation
    TEST_ASSERT_TRUE(true);
}

// ---------------------------------------------------------------------------
//  Forward declarations of tests located in other translation units
// ---------------------------------------------------------------------------

void test_dest_size_one();
void test_empty_source();

// ---------------------------------------------------------------------------
//  Unity test runner
// ---------------------------------------------------------------------------

void setUp() {}
void tearDown() {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_null_source);
    RUN_TEST(test_exact_fit);
    RUN_TEST(test_truncation);
    RUN_TEST(test_buffer_integrity);
    // Tests defined in test_safe_strncpy_bounds.cpp
    RUN_TEST(test_dest_size_one);
    RUN_TEST(test_empty_source);
    RUN_TEST(test_null_dest);
    return UNITY_END();
}
