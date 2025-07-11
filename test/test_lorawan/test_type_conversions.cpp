#include <unity.h>
#include "lorawan.hpp"

// Mock sensor data structure for testing
struct MockSensorData {
    float soilMoisturePercentage;
    float soilMoistureValue;
    bool lipoGaugeOk;
    float vBat;
    float batPercent;
    float batRate;
};

void test_float_conversions() {
    // Test that float conversions work correctly
    int intValue = 1234;
    long longValue = 5678L;
    
    float floatFromInt = static_cast<float>(intValue);
    float floatFromLong = static_cast<float>(longValue);
    
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 1234.0f, floatFromInt);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 5678.0f, floatFromLong);
}

void test_uint32_conversions() {
    // Test that uint32_t conversions work correctly
    float floatValue = 123.456f;
    uint32_t uintFromFloat = static_cast<uint32_t>(floatValue);
    
    TEST_ASSERT_EQUAL(123, uintFromFloat);
}

void test_sensor_data_initialization() {
    // Test that sensor data can be initialized with proper types
    MockSensorData sd;
    sd.soilMoisturePercentage = 0.0f;
    sd.soilMoistureValue = 0.0f;
    sd.lipoGaugeOk = false;
    sd.vBat = 0.0f;
    sd.batPercent = 0.0f;
    sd.batRate = 0.0f;
    
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sd.soilMoisturePercentage);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sd.soilMoistureValue);
    TEST_ASSERT_FALSE(sd.lipoGaugeOk);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sd.vBat);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sd.batPercent);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sd.batRate);
}

void test_array_access() {
    // Test that array access works correctly with proper types
    const char* const testArray[] = {"test1", "test2"};
    TEST_ASSERT_EQUAL_STRING("test1", testArray[0]);
    TEST_ASSERT_EQUAL_STRING("test2", testArray[1]);
}

// Remove setUp and tearDown from this file 