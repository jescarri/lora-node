#include <unity.h>
#include "utils.hpp"

void test_dest_size_one() {
    char dest[1];
    safe_strncpy(dest, "ABC"); // should truncate to empty string but remain null-terminated
    TEST_ASSERT_EQUAL_CHAR('\0', dest[0]);
}

void test_empty_source() {
    constexpr char kSrc[] = ""; // already empty
    char dest[4] = {'x', 'x', 'x', 'x'};
    safe_strncpy(dest, kSrc);
    TEST_ASSERT_EQUAL_STRING(kSrc, dest);
    // Ensure the remaining bytes were not overwritten past terminator
    TEST_ASSERT_EQUAL_CHAR('x', dest[1]);
}

// setUp and tearDown as well as the Unity test runner `main` are
// provided by another translation unit in this test suite
// (see `test_safe_strncpy.cpp`). Defining them here as well would result
// in multiple-definition linker errors when PlatformIO links all source
// files in the `test_utils` suite into a single executable. Therefore we
// only declare the test cases in this file – the shared runner will pick
// them up automatically via `RUN_TEST()` calls added there.
