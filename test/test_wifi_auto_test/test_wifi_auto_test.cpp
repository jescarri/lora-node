#include <unity.h>
#include <string>
#include <map>

// Mock settings storage
std::map<std::string, std::string> mock_settings_string;
std::map<std::string, bool> mock_settings_bool;

// Mock functions
void settings_put_string(const std::string& key, const std::string& value) {
    mock_settings_string[key] = value;
}

void settings_put_bool(const std::string& key, bool value) {
    mock_settings_bool[key] = value;
}

std::string settings_get_string(const std::string& key, const std::string& default_value = "") {
    auto it = mock_settings_string.find(key);
    return (it != mock_settings_string.end()) ? it->second : default_value;
}

bool settings_get_bool(const std::string& key, bool default_value = false) {
    auto it = mock_settings_bool.find(key);
    return (it != mock_settings_bool.end()) ? it->second : default_value;
}

// Mock WiFi test function
bool testWifiConnection(const std::string& ssid, const std::string& password) {
    // Mock implementation - return true for valid credentials
    return !ssid.empty() && !password.empty();
}

// Test helper function to simulate the WiFi testing logic
bool shouldTestWiFi(const std::string& current_ssid, const std::string& current_password) {
    bool wifi_tested_ok = settings_get_bool("wifi_tested_ok", false);
    
    // Get previous WiFi settings for comparison
    std::string prev_ssid = settings_get_string("wifi_ssid", "");
    std::string prev_password = settings_get_string("wifi_password", "");
    
    // Test WiFi if:
    // 1. wifi_tested_ok is false/nil and SSID is provided
    // 2. WiFi config has changed
    if (current_ssid.length() > 0) {
        if (!wifi_tested_ok) {
            return true;
        } else if (current_ssid != prev_ssid || current_password != prev_password) {
            return true;
        }
    }
    return false;
}

void setUp(void) {
    // Clear mock settings before each test
    mock_settings_string.clear();
    mock_settings_bool.clear();
}

void tearDown(void) {
    // Cleanup after each test
    mock_settings_string.clear();
    mock_settings_bool.clear();
}

void test_wifi_test_when_not_tested_before() {
    // Test case: WiFi has never been tested before
    std::string ssid = "test_network";
    std::string password = "test_password";
    
    // wifi_tested_ok should be false by default
    bool should_test = shouldTestWiFi(ssid, password);
    TEST_ASSERT_TRUE(should_test);
}

void test_wifi_test_when_config_changed() {
    // Test case: WiFi config has changed
    std::string old_ssid = "old_network";
    std::string old_password = "old_password";
    std::string new_ssid = "new_network";
    std::string new_password = "new_password";
    
    // Set up previous config and mark as tested
    settings_put_string("wifi_ssid", old_ssid);
    settings_put_string("wifi_password", old_password);
    settings_put_bool("wifi_tested_ok", true);
    
    // Test with new config
    bool should_test = shouldTestWiFi(new_ssid, new_password);
    TEST_ASSERT_TRUE(should_test);
}

void test_wifi_skip_when_config_unchanged() {
    // Test case: WiFi config unchanged and previously tested
    std::string ssid = "test_network";
    std::string password = "test_password";
    
    // Set up same config and mark as tested
    settings_put_string("wifi_ssid", ssid);
    settings_put_string("wifi_password", password);
    settings_put_bool("wifi_tested_ok", true);
    
    // Test with same config
    bool should_test = shouldTestWiFi(ssid, password);
    TEST_ASSERT_FALSE(should_test);
}

void test_wifi_skip_when_no_ssid() {
    // Test case: No SSID provided
    std::string ssid = "";
    std::string password = "test_password";
    
    bool should_test = shouldTestWiFi(ssid, password);
    TEST_ASSERT_FALSE(should_test);
}

void test_wifi_test_when_password_changed() {
    // Test case: SSID same but password changed
    std::string ssid = "test_network";
    std::string old_password = "old_password";
    std::string new_password = "new_password";
    
    // Set up previous config and mark as tested
    settings_put_string("wifi_ssid", ssid);
    settings_put_string("wifi_password", old_password);
    settings_put_bool("wifi_tested_ok", true);
    
    // Test with new password
    bool should_test = shouldTestWiFi(ssid, new_password);
    TEST_ASSERT_TRUE(should_test);
}

void test_wifi_test_success_updates_flag() {
    // Test case: WiFi test success should update the flag
    std::string ssid = "test_network";
    std::string password = "test_password";
    
    // Initially not tested
    TEST_ASSERT_FALSE(settings_get_bool("wifi_tested_ok", false));
    
    // Simulate successful test
    if (testWifiConnection(ssid, password)) {
        settings_put_bool("wifi_tested_ok", true);
    }
    
    TEST_ASSERT_TRUE(settings_get_bool("wifi_tested_ok", false));
}

void test_wifi_test_failure_updates_flag() {
    // Test case: WiFi test failure should update the flag
    std::string ssid = "";
    std::string password = "test_password";
    
    // Initially tested successfully
    settings_put_bool("wifi_tested_ok", true);
    
    // Simulate failed test
    if (testWifiConnection(ssid, password)) {
        settings_put_bool("wifi_tested_ok", true);
    } else {
        settings_put_bool("wifi_tested_ok", false);
    }
    
    TEST_ASSERT_FALSE(settings_get_bool("wifi_tested_ok", true));
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_wifi_test_when_not_tested_before);
    RUN_TEST(test_wifi_test_when_config_changed);
    RUN_TEST(test_wifi_skip_when_config_unchanged);
    RUN_TEST(test_wifi_skip_when_no_ssid);
    RUN_TEST(test_wifi_test_when_password_changed);
    RUN_TEST(test_wifi_test_success_updates_flag);
    RUN_TEST(test_wifi_test_failure_updates_flag);
    
    return UNITY_END();
} 