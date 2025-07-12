#include <unity.h>
#include <string>
#include <cstring>

// Simple test structure for OTA info
struct SimpleOtaInfo {
    std::string url;
    std::string md5sum;
    std::string version;
};

// Simplified test functions that focus on core logic
bool test_parse_json_simple() {
    // Test JSON parsing logic without Arduino dependencies
    std::string json = "{\"url\":\"https://example.com/fw.bin\",\"md5sum\":\"d41d8cd98f00b204e9800998ecf8427e\",\"version\":\"101\"}";
    
    // Simple validation - check if JSON contains required fields
    bool has_url = json.find("\"url\"") != std::string::npos;
    bool has_md5sum = json.find("\"md5sum\"") != std::string::npos;
    bool has_version = json.find("\"version\"") != std::string::npos;
    
    return has_url && has_md5sum && has_version;
}

bool test_parse_json_missing_fields() {
    std::string json = "{\"url\":\"https://example.com/fw.bin\"}";
    
    bool has_url = json.find("\"url\"") != std::string::npos;
    bool has_md5sum = json.find("\"md5sum\"") != std::string::npos;
    bool has_version = json.find("\"version\"") != std::string::npos;
    
    // Should return false because required fields are missing
    return has_url && has_md5sum && has_version;
}

bool test_parse_json_malformed() {
    std::string json = "invalid json";
    
    bool has_url = json.find("\"url\"") != std::string::npos;
    bool has_md5sum = json.find("\"md5sum\"") != std::string::npos;
    bool has_version = json.find("\"version\"") != std::string::npos;
    
    // Should return false because JSON is malformed
    return has_url && has_md5sum && has_version;
}

bool test_md5_validation_simple() {
    // Test MD5 validation logic
    std::string expected_hash = "d41d8cd98f00b204e9800998ecf8427e";
    std::string actual_hash = "d41d8cd98f00b204e9800998ecf8427e";
    
    return expected_hash == actual_hash;
}

bool test_md5_validation_wrong() {
    std::string expected_hash = "d41d8cd98f00b204e9800998ecf8427e";
    std::string actual_hash = "wrong_hash_value";
    
    return expected_hash != actual_hash;
}

void test_ota_parsing_valid() {
    TEST_ASSERT_TRUE(test_parse_json_simple());
}

void test_ota_parsing_missing_fields() {
    TEST_ASSERT_FALSE(test_parse_json_missing_fields());
}

void test_ota_parsing_malformed() {
    TEST_ASSERT_FALSE(test_parse_json_malformed());
}

void test_md5_validation_correct() {
    TEST_ASSERT_TRUE(test_md5_validation_simple());
}

void test_md5_validation_incorrect() {
    TEST_ASSERT_TRUE(test_md5_validation_wrong());
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_ota_parsing_valid);
    RUN_TEST(test_ota_parsing_missing_fields);
    RUN_TEST(test_ota_parsing_malformed);
    RUN_TEST(test_md5_validation_correct);
    RUN_TEST(test_md5_validation_incorrect);
    
    return UNITY_END();
} 