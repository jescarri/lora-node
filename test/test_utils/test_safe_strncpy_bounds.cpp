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

void setUp() {}
void tearDown() {}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_dest_size_one);
    RUN_TEST(test_empty_source);
    return UNITY_END();
}

