#include <unity.h>
#include <string>
#include <cstring>
#include <vector> // Added for base64 tests

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

// --- Base64 decode helper for tests ---
std::string test_b64decode(const std::string& in) {
    static const std::string b64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[b64_chars[i]] = i;
    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

void test_base64_decode_valid() {
    TEST_ASSERT_EQUAL_STRING("hello", test_b64decode("aGVsbG8=").c_str());
    TEST_ASSERT_EQUAL_STRING("foobar", test_b64decode("Zm9vYmFy").c_str());
    TEST_ASSERT_EQUAL_STRING("", test_b64decode("").c_str());
}

void test_base64_decode_padding() {
    TEST_ASSERT_EQUAL_STRING("any carnal pleasure.", test_b64decode("YW55IGNhcm5hbCBwbGVhc3VyZS4=").c_str());
    TEST_ASSERT_EQUAL_STRING("any carnal pleasure", test_b64decode("YW55IGNhcm5hbCBwbGVhc3VyZQ==").c_str());
    TEST_ASSERT_EQUAL_STRING("any carnal pleasur", test_b64decode("YW55IGNhcm5hbCBwbGVhc3Vy").c_str());
}

void test_base64_decode_invalid() {
    // Should stop at first invalid char
    TEST_ASSERT_EQUAL_STRING("", test_b64decode("!@#$").c_str());
    // Partial valid
    TEST_ASSERT_EQUAL_STRING("f", test_b64decode("Zg!@#").c_str());
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
    RUN_TEST(test_base64_decode_valid);
    RUN_TEST(test_base64_decode_padding);
    RUN_TEST(test_base64_decode_invalid);
    
    return UNITY_END();
} 