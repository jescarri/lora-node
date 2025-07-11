#include <unity.h>
#include <memory>
#include <string>

// Mock WiFiManagerParameter for testing
class MockWiFiManagerParameter {
public:
    MockWiFiManagerParameter(const char* id, const char* label, const char* value, int length)
        : id_(id), label_(label), value_(value), length_(length) {}
    
    const char* getValue() const { return value_.c_str(); }
    const char* getID() const { return id_.c_str(); }
    const char* getLabel() const { return label_.c_str(); }
    int getValueLength() const { return length_; }

private:
    std::string id_;
    std::string label_;
    std::string value_;
    int length_;
};

void test_unique_ptr_creation() {
    // Test that unique_ptr can be created and managed properly
    auto param = std::make_unique<MockWiFiManagerParameter>(
        "test_id", "Test Label", "test_value", 20);
    
    TEST_ASSERT_NOT_NULL(param.get());
    TEST_ASSERT_EQUAL_STRING("test_id", param->getID());
    TEST_ASSERT_EQUAL_STRING("Test Label", param->getLabel());
    TEST_ASSERT_EQUAL_STRING("test_value", param->getValue());
    TEST_ASSERT_EQUAL(20, param->getValueLength());
}

void test_unique_ptr_array() {
    // Test that arrays of unique_ptr work correctly
    std::unique_ptr<MockWiFiManagerParameter> params[3];
    
    params[0] = std::make_unique<MockWiFiManagerParameter>("id1", "Label1", "value1", 10);
    params[1] = std::make_unique<MockWiFiManagerParameter>("id2", "Label2", "value2", 10);
    params[2] = std::make_unique<MockWiFiManagerParameter>("id3", "Label3", "value3", 10);
    
    TEST_ASSERT_NOT_NULL(params[0].get());
    TEST_ASSERT_NOT_NULL(params[1].get());
    TEST_ASSERT_NOT_NULL(params[2].get());
    
    TEST_ASSERT_EQUAL_STRING("id1", params[0]->getID());
    TEST_ASSERT_EQUAL_STRING("id2", params[1]->getID());
    TEST_ASSERT_EQUAL_STRING("id3", params[2]->getID());
}

void test_const_array_access() {
    // Test that const arrays work correctly
    const char* const testMenu[] = {"param", "restart"};
    
    TEST_ASSERT_EQUAL_STRING("param", testMenu[0]);
    TEST_ASSERT_EQUAL_STRING("restart", testMenu[1]);
    TEST_ASSERT_EQUAL(2, sizeof(testMenu) / sizeof(testMenu[0]));
}

void test_string_operations() {
    // Test that string operations work correctly
    std::string testString = "test_value";
    char buffer[20];
    
    // Simulate safe_strncpy behavior
    size_t copyLen = std::min(testString.length(), sizeof(buffer) - 1);
    testString.copy(buffer, copyLen);
    buffer[copyLen] = '\0';
    
    TEST_ASSERT_EQUAL_STRING("test_value", buffer);
}

void setUp() {}
void tearDown() {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_unique_ptr_creation);
    RUN_TEST(test_unique_ptr_array);
    RUN_TEST(test_const_array_access);
    RUN_TEST(test_string_operations);
    return UNITY_END();
} 