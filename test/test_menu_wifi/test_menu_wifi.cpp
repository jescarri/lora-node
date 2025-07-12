#include <unity.h>
#include "menu.hpp"
#include "lorawan_settings.hpp"
#include <cstring>

void setUp(void) {
    // Clear any existing settings
    settings_clear();
}

void tearDown(void) {
    // Cleanup
    settings_clear();
}

void test_wifi_settings_loading_empty() {
    // Test loading when no WiFi settings exist
    loadSetings();
    
    // Check that default values are set
#ifdef UNIT_TEST
    TEST_ASSERT_EQUAL_STRING("", wifi_ssid_str);
    TEST_ASSERT_EQUAL_STRING("", wifi_password_str);
#else
    TEST_ASSERT_EQUAL_STRING("", wifi_ssid_str);
    TEST_ASSERT_EQUAL_STRING("", wifi_password_str);
#endif
}

void test_wifi_settings_loading_with_values() {
    // Set some WiFi settings
    settings_put_string("wifi_ssid", "TestNetwork");
    settings_put_string("wifi_password", "TestPassword123");
    
    // Load settings
    loadSetings();
    
    // Check that values are loaded correctly
#ifdef UNIT_TEST
    TEST_ASSERT_EQUAL_STRING("TestNetwork", wifi_ssid_str);
    TEST_ASSERT_EQUAL_STRING("TestPassword123", wifi_password_str);
#else
    TEST_ASSERT_EQUAL_STRING("TestNetwork", wifi_ssid_str);
    TEST_ASSERT_EQUAL_STRING("TestPassword123", wifi_password_str);
#endif
}

void test_wifi_settings_saving() {
    // Initialize menu to create parameters
    initMenu();
    
    // Simulate setting values
    strcpy(wifi_ssid_str, "NewNetwork");
    strcpy(wifi_password_str, "NewPassword456");
    
    // Call save callback
    saveConfigCallback();
    
    // Check that settings were saved
    TEST_ASSERT_TRUE(settings_has_key("wifi_ssid"));
    TEST_ASSERT_TRUE(settings_has_key("wifi_password"));
    TEST_ASSERT_EQUAL_STRING("NewNetwork", settings_get_string("wifi_ssid").c_str());
    TEST_ASSERT_EQUAL_STRING("NewPassword456", settings_get_string("wifi_password").c_str());
}

void test_wifi_settings_partial_save() {
    // Initialize menu to create parameters
    initMenu();
    
    // Simulate setting only SSID
    strcpy(wifi_ssid_str, "NetworkOnly");
    strcpy(wifi_password_str, "");
    
    // Call save callback
    saveConfigCallback();
    
    // Check that settings were saved
    TEST_ASSERT_TRUE(settings_has_key("wifi_ssid"));
    TEST_ASSERT_TRUE(settings_has_key("wifi_password"));
    TEST_ASSERT_EQUAL_STRING("NetworkOnly", settings_get_string("wifi_ssid").c_str());
    TEST_ASSERT_EQUAL_STRING("", settings_get_string("wifi_password").c_str());
}

void test_wifi_settings_persistence() {
    // Set initial values
    settings_put_string("wifi_ssid", "PersistentNetwork");
    settings_put_string("wifi_password", "PersistentPassword");
    
    // Load settings
    loadSetings();
    
    // Verify values are loaded
#ifdef UNIT_TEST
    TEST_ASSERT_EQUAL_STRING("PersistentNetwork", wifi_ssid_str);
    TEST_ASSERT_EQUAL_STRING("PersistentPassword", wifi_password_str);
#else
    TEST_ASSERT_EQUAL_STRING("PersistentNetwork", wifi_ssid_str);
    TEST_ASSERT_EQUAL_STRING("PersistentPassword", wifi_password_str);
#endif
    
    // Change values and save
    initMenu();
    strcpy(wifi_ssid_str, "UpdatedNetwork");
    strcpy(wifi_password_str, "UpdatedPassword");
    saveConfigCallback();
    
    // Verify new values are saved
    TEST_ASSERT_EQUAL_STRING("UpdatedNetwork", settings_get_string("wifi_ssid").c_str());
    TEST_ASSERT_EQUAL_STRING("UpdatedPassword", settings_get_string("wifi_password").c_str());
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_wifi_settings_loading_empty);
    RUN_TEST(test_wifi_settings_loading_with_values);
    RUN_TEST(test_wifi_settings_saving);
    RUN_TEST(test_wifi_settings_partial_save);
    RUN_TEST(test_wifi_settings_persistence);
    
    return UNITY_END();
} 