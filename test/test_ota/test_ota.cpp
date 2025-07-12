#include <unity.h>
#include "ota.hpp"
#include "lorawan_settings.hpp"
#include <cstring>

// Test data
const char* validOtaJson = "{\"url\":\"https://example.com/firmware.bin\",\"md5sum\":\"d41d8cd98f00b204e9800998ecf8427e\",\"version\":\"101\"}";
const char* invalidOtaJson = "{\"url\":\"https://example.com/firmware.bin\"}";
const char* malformedJson = "{invalid json}";

void setUp(void) {
    // Setup code if needed
}

void tearDown(void) {
    // Cleanup code if needed
}

void test_parse_valid_ota_message() {
    OtaUpdateInfo updateInfo;
    uint8_t data[256];
    strcpy((char*)data, validOtaJson);
    uint8_t dataLen = strlen(validOtaJson);
    
    bool result = parseOtaMessage(data, dataLen, updateInfo);
    
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(updateInfo.valid);
    TEST_ASSERT_EQUAL_STRING("https://example.com/firmware.bin", updateInfo.url.c_str());
    TEST_ASSERT_EQUAL_STRING("d41d8cd98f00b204e9800998ecf8427e", updateInfo.md5sum.c_str());
    TEST_ASSERT_EQUAL_STRING("101", updateInfo.version.c_str());
}

void test_parse_invalid_ota_message_missing_fields() {
    OtaUpdateInfo updateInfo;
    uint8_t data[256];
    strcpy((char*)data, invalidOtaJson);
    uint8_t dataLen = strlen(invalidOtaJson);
    
    bool result = parseOtaMessage(data, dataLen, updateInfo);
    
    TEST_ASSERT_FALSE(result);
}

void test_parse_malformed_json() {
    OtaUpdateInfo updateInfo;
    uint8_t data[256];
    strcpy((char*)data, malformedJson);
    uint8_t dataLen = strlen(malformedJson);
    
    bool result = parseOtaMessage(data, dataLen, updateInfo);
    
    TEST_ASSERT_FALSE(result);
}

void test_parse_null_data() {
    OtaUpdateInfo updateInfo;
    
    bool result = parseOtaMessage(nullptr, 0, updateInfo);
    
    TEST_ASSERT_FALSE(result);
}

void test_parse_empty_data() {
    OtaUpdateInfo updateInfo;
    uint8_t data[1] = {0};
    
    bool result = parseOtaMessage(data, 0, updateInfo);
    
    TEST_ASSERT_FALSE(result);
}

void test_firmware_version_constant() {
    // Test that FIRMWARE_VERSION is defined and has a reasonable value
    TEST_ASSERT_GREATER_THAN(0, FIRMWARE_VERSION_FLOAT);
    TEST_ASSERT_LESS_THAN(10000, FIRMWARE_VERSION_FLOAT); // Reasonable upper bound
}

void test_verify_md5_sum() {
    const char* testData = "Hello, World!";
    const char* expectedMd5 = "65a8e27d8879283831b664bd8b7f0ad4";
    
    bool result = verifyMd5Sum((uint8_t*)testData, strlen(testData), expectedMd5);
    
    TEST_ASSERT_TRUE(result);
}

void test_verify_md5_sum_wrong_hash() {
    const char* testData = "Hello, World!";
    const char* wrongMd5 = "00000000000000000000000000000000";
    
    bool result = verifyMd5Sum((uint8_t*)testData, strlen(testData), wrongMd5);
    
    TEST_ASSERT_FALSE(result);
}

void test_verify_md5_sum_empty_data() {
    const char* expectedMd5 = "d41d8cd98f00b204e9800998ecf8427e";
    
    bool result = verifyMd5Sum(nullptr, 0, expectedMd5);
    
    TEST_ASSERT_FALSE(result);
}

void test_verify_md5_sum_empty_hash() {
    const char* testData = "Hello, World!";
    
    bool result = verifyMd5Sum((uint8_t*)testData, strlen(testData), "");
    
    TEST_ASSERT_FALSE(result);
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_parse_valid_ota_message);
    RUN_TEST(test_parse_invalid_ota_message_missing_fields);
    RUN_TEST(test_parse_malformed_json);
    RUN_TEST(test_parse_null_data);
    RUN_TEST(test_parse_empty_data);
    RUN_TEST(test_firmware_version_constant);
    RUN_TEST(test_verify_md5_sum);
    RUN_TEST(test_verify_md5_sum_wrong_hash);
    RUN_TEST(test_verify_md5_sum_empty_data);
    RUN_TEST(test_verify_md5_sum_empty_hash);
    
    return UNITY_END();
} 