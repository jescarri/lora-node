#include <unity.h>
#include <string>
#include <map>

// Simple in-memory settings storage for testing
class SimpleSettings {
private:
    std::map<std::string, std::string> storage;

public:
    void putString(const std::string& key, const std::string& value) {
        storage[key] = value;
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") {
        auto it = storage.find(key);
        return (it != storage.end()) ? it->second : defaultValue;
    }
    
    bool hasKey(const std::string& key) {
        return storage.find(key) != storage.end();
    }
    
    void clear() {
        storage.clear();
    }
};

// Simple WiFi settings structure
struct SimpleWifiSettings {
    std::string ssid;
    std::string password;
};

// Test functions
void test_wifi_settings_save_load() {
    SimpleSettings settings;
    SimpleWifiSettings wifi;
    
    // Test saving
    wifi.ssid = "TestNetwork";
    wifi.password = "TestPassword123";
    
    settings.putString("wifi_ssid", wifi.ssid);
    settings.putString("wifi_password", wifi.password);
    
    // Test loading
    std::string loaded_ssid = settings.getString("wifi_ssid");
    std::string loaded_password = settings.getString("wifi_password");
    
    TEST_ASSERT_EQUAL_STRING("TestNetwork", loaded_ssid.c_str());
    TEST_ASSERT_EQUAL_STRING("TestPassword123", loaded_password.c_str());
}

void test_wifi_settings_empty() {
    SimpleSettings settings;
    
    std::string ssid = settings.getString("wifi_ssid");
    std::string password = settings.getString("wifi_password");
    
    TEST_ASSERT_EQUAL_STRING("", ssid.c_str());
    TEST_ASSERT_EQUAL_STRING("", password.c_str());
}

void test_wifi_settings_partial() {
    SimpleSettings settings;
    
    // Only set SSID
    settings.putString("wifi_ssid", "NetworkOnly");
    
    std::string ssid = settings.getString("wifi_ssid");
    std::string password = settings.getString("wifi_password");
    
    TEST_ASSERT_EQUAL_STRING("NetworkOnly", ssid.c_str());
    TEST_ASSERT_EQUAL_STRING("", password.c_str());
}

void test_wifi_settings_persistence() {
    SimpleSettings settings;
    
    // Set initial values
    settings.putString("wifi_ssid", "InitialNetwork");
    settings.putString("wifi_password", "InitialPassword");
    
    // Verify initial values
    TEST_ASSERT_EQUAL_STRING("InitialNetwork", settings.getString("wifi_ssid").c_str());
    TEST_ASSERT_EQUAL_STRING("InitialPassword", settings.getString("wifi_password").c_str());
    
    // Update values
    settings.putString("wifi_ssid", "UpdatedNetwork");
    settings.putString("wifi_password", "UpdatedPassword");
    
    // Verify updated values
    TEST_ASSERT_EQUAL_STRING("UpdatedNetwork", settings.getString("wifi_ssid").c_str());
    TEST_ASSERT_EQUAL_STRING("UpdatedPassword", settings.getString("wifi_password").c_str());
}

void test_wifi_settings_clear() {
    SimpleSettings settings;
    
    // Set some values
    settings.putString("wifi_ssid", "TestNetwork");
    settings.putString("wifi_password", "TestPassword");
    
    // Verify values exist
    TEST_ASSERT_TRUE(settings.hasKey("wifi_ssid"));
    TEST_ASSERT_TRUE(settings.hasKey("wifi_password"));
    
    // Clear settings
    settings.clear();
    
    // Verify values are gone
    TEST_ASSERT_FALSE(settings.hasKey("wifi_ssid"));
    TEST_ASSERT_FALSE(settings.hasKey("wifi_password"));
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_wifi_settings_save_load);
    RUN_TEST(test_wifi_settings_empty);
    RUN_TEST(test_wifi_settings_partial);
    RUN_TEST(test_wifi_settings_persistence);
    RUN_TEST(test_wifi_settings_clear);
    
    return UNITY_END();
} 