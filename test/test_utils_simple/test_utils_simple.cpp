#include <unity.h>
#include <string>
#include <cstring>

// Simple safe string copy function for testing
void safe_strncpy(char* dest, const char* src, size_t dest_size) {
    if (dest == nullptr || dest_size == 0) {
        return;
    }
    
    if (src == nullptr) {
        dest[0] = '\0';
        return;
    }
    
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
    
    strncpy(dest, src, copy_len);
    dest[copy_len] = '\0';
}

// Test functions
void test_safe_strncpy_normal() {
    char dest[10];
    const char* src = "Hello";
    
    safe_strncpy(dest, src, sizeof(dest));
    
    TEST_ASSERT_EQUAL_STRING("Hello", dest);
}

void test_safe_strncpy_truncation() {
    char dest[5];
    const char* src = "Hello World";
    
    safe_strncpy(dest, src, sizeof(dest));
    
    TEST_ASSERT_EQUAL_STRING("Hell", dest);
    TEST_ASSERT_EQUAL(4, strlen(dest));
}

void test_safe_strncpy_null_dest() {
    const char* src = "Hello";
    
    // Should not crash
    safe_strncpy(nullptr, src, 10);
    
    TEST_ASSERT_TRUE(true); // If we get here, no crash occurred
}

void test_safe_strncpy_null_src() {
    char dest[10];
    
    safe_strncpy(dest, nullptr, sizeof(dest));
    
    TEST_ASSERT_EQUAL_STRING("", dest);
}

void test_safe_strncpy_zero_size() {
    char dest[10];
    const char* src = "Hello";
    
    safe_strncpy(dest, src, 0);
    
    // dest should remain unchanged (implementation dependent)
    TEST_ASSERT_TRUE(true); // If we get here, no crash occurred
}

void test_safe_strncpy_exact_fit() {
    char dest[6]; // "Hello" + null terminator
    const char* src = "Hello";
    
    safe_strncpy(dest, src, sizeof(dest));
    
    TEST_ASSERT_EQUAL_STRING("Hello", dest);
}

void test_safe_strncpy_empty_src() {
    char dest[10];
    const char* src = "";
    
    safe_strncpy(dest, src, sizeof(dest));
    
    TEST_ASSERT_EQUAL_STRING("", dest);
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_safe_strncpy_normal);
    RUN_TEST(test_safe_strncpy_truncation);
    RUN_TEST(test_safe_strncpy_null_dest);
    RUN_TEST(test_safe_strncpy_null_src);
    RUN_TEST(test_safe_strncpy_zero_size);
    RUN_TEST(test_safe_strncpy_exact_fit);
    RUN_TEST(test_safe_strncpy_empty_src);
    
    return UNITY_END();
} 