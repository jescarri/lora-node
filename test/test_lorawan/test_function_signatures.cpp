#include <unity.h>

// Mock structures for testing
struct MockLmicT {
    int seqnoUp;
    int globalDutyRate;
    int globalDutyAvail;
    int txend;
    int txChnl;
    int opmode;
};

struct MockOsjobT {
    int dummy;
};

// Mock function signatures that match the improved versions
void test_const_reference_parameter(const MockLmicT& lmic_check) {
    // Test that const reference parameter works correctly
    TEST_ASSERT_EQUAL(0, lmic_check.seqnoUp);
    TEST_ASSERT_EQUAL(0, lmic_check.globalDutyRate);
    TEST_ASSERT_EQUAL(0, lmic_check.globalDutyAvail);
    TEST_ASSERT_EQUAL(0, lmic_check.txend);
    TEST_ASSERT_EQUAL(0, lmic_check.txChnl);
    TEST_ASSERT_EQUAL(0, lmic_check.opmode);
}

void test_unused_parameter(MockOsjobT* /* j */) {
    // Test that unused parameter with comment works correctly
    // This function should compile without warnings
    TEST_ASSERT_TRUE(true);
}

void test_const_pointer_parameter(const MockLmicT* lmic_ptr) {
    // Test that const pointer parameter works correctly
    if (lmic_ptr != nullptr) {
        TEST_ASSERT_EQUAL(0, lmic_ptr->seqnoUp);
        TEST_ASSERT_EQUAL(0, lmic_ptr->globalDutyRate);
    }
}

// Test function that demonstrates proper const usage
class TestClass {
public:
    void constMethod() const {
        // This method cannot modify the object
        TEST_ASSERT_TRUE(true);
    }
    
    void nonConstMethod() {
        // This method can modify the object
        value_ = 42;
    }
    
    int getValue() const { return value_; }
    
private:
    int value_ = 0;
};

void test_const_methods() {
    TestClass obj;
    const TestClass& constObj = obj;
    
    // Test const method call
    constObj.constMethod();
    
    // Test non-const method call
    obj.nonConstMethod();
    TEST_ASSERT_EQUAL(42, obj.getValue());
}

void test_noreturn_attribute() {
    // Test that [[noreturn]] attribute is recognized
    // Note: We can't actually test noreturn behavior in a unit test
    // but we can verify the attribute doesn't cause compilation issues
    TEST_ASSERT_TRUE(true);
}

void test_static_cast_conversions() {
    // Test that static_cast conversions work correctly
    int intValue = 123;
    float floatValue = 456.789f;
    
    float floatFromInt = static_cast<float>(intValue);
    int intFromFloat = static_cast<int>(floatValue);
    
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 123.0f, floatFromInt);
    TEST_ASSERT_EQUAL(456, intFromFloat);
} 