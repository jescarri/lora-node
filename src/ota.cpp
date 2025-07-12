#include "ota.hpp"
#include "lorawan_settings.hpp"
#include <cstring>
#ifndef UNIT_TEST
#include <mbedtls/md5.h>
#endif

void handleDownlinkMessage(uint8_t* data, uint8_t dataLen) {
    Serial.println("Handling downlink message");
    
    OtaUpdateInfo updateInfo;
    if (parseOtaMessage(data, dataLen, updateInfo)) {
        Serial.println("Valid OTA message received");
        Serial.print("URL: "); Serial.println(updateInfo.url);
        Serial.print("MD5: "); Serial.println(updateInfo.md5sum);
        Serial.print("Version: "); Serial.println(updateInfo.version);
        
#ifndef UNIT_TEST
        if (downloadAndInstallFirmware(updateInfo)) {
            Serial.println("OTA update completed successfully");
            // Restart the device to apply the update
            ESP.restart();
        } else {
            Serial.println("OTA update failed");
        }
#else
        Serial.println("OTA update simulated (unit test)");
#endif
    } else {
        Serial.println("Invalid OTA message format");
    }
}

bool parseOtaMessage(const uint8_t* data, uint8_t dataLen, OtaUpdateInfo& updateInfo) {
    if (data == nullptr || dataLen == 0) {
        return false;
    }
    
    // Convert data to string for JSON parsing
    String jsonString = String((char*)data, dataLen);
    Serial.print("Received JSON: "); Serial.println(jsonString);
    
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, jsonString);
    
    if (error) {
        Serial.print("JSON parsing failed: "); Serial.println(error.c_str());
        return false;
    }
    
    // Check if all required fields are present
    if (!doc.containsKey("url") || !doc.containsKey("md5sum") || !doc.containsKey("version")) {
        Serial.println("Missing required fields in OTA message");
        return false;
    }
    
    updateInfo.url = doc["url"].as<String>();
    updateInfo.md5sum = doc["md5sum"].as<String>();
    updateInfo.version = doc["version"].as<String>();
    updateInfo.valid = true;
    
    return true;
}

#ifndef UNIT_TEST
bool downloadAndInstallFirmware(const OtaUpdateInfo& updateInfo) {
    if (!updateInfo.valid) {
        Serial.println("Invalid update info");
        return false;
    }
    
    // Get saved WiFi credentials
    String ssid = settings_get_string("wifi_ssid");
    String password = settings_get_string("wifi_password");
    
    if (ssid.length() == 0) {
        Serial.println("No WiFi credentials configured");
        return false;
    }
    
    // Enable WiFi for download
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Wait for WiFi connection
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to connect to WiFi");
        return false;
    }
    
    Serial.println("WiFi connected");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
    
    // Download firmware
    HTTPClient http;
    http.begin(updateInfo.url);
    
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }
    
    int contentLength = http.getSize();
    Serial.printf("Content length: %d\n", contentLength);
    
    if (contentLength <= 0) {
        Serial.println("Invalid content length");
        http.end();
        return false;
    }
    
    // Check if we have enough space for the update
    if (contentLength > UPDATE_SIZE_UNKNOWN) {
        Serial.println("Firmware too large");
        http.end();
        return false;
    }
    
    // Start update
    if (!Update.begin(contentLength)) {
        Serial.println("Not enough space to begin OTA");
        http.end();
        return false;
    }
    
    // Get the update stream
    WiFiClient* stream = http.getStreamPtr();
    
    // Write firmware data
    size_t written = Update.writeStream(*stream);
    if (written != contentLength) {
        Serial.printf("Written %d of %d bytes\n", written, contentLength);
        http.end();
        return false;
    }
    
    http.end();
    
    // Verify MD5 if provided
    if (updateInfo.md5sum.length() > 0) {
        if (!Update.setMD5(updateInfo.md5sum.c_str())) {
            Serial.println("Failed to set MD5");
            return false;
        }
    }
    
    // End update
    if (!Update.end()) {
        Serial.println("Update end failed");
        return false;
    }
    
    Serial.println("OTA update completed successfully");
    return true;
}
#else
// Stub for unit test
bool downloadAndInstallFirmware(const OtaUpdateInfo&) {
    return true;
}
#endif

#ifndef UNIT_TEST
#include <mbedtls/md5.h>
bool verifyMd5Sum(const uint8_t* data, size_t dataLen, const String& expectedMd5) {
    if (data == nullptr || dataLen == 0 || expectedMd5.length() == 0) {
        return false;
    }
    
    unsigned char hash[16];
    mbedtls_md5(data, dataLen, hash);
    
    String calculatedMd5 = "";
    for (int i = 0; i < 16; i++) {
        char hex[3];
        sprintf(hex, "%02x", hash[i]);
        calculatedMd5 += hex;
    }
    
    Serial.print("Calculated MD5: "); Serial.println(calculatedMd5);
    Serial.print("Expected MD5: "); Serial.println(expectedMd5);
    
    return calculatedMd5.equalsIgnoreCase(expectedMd5);
}
#else
// Stub for unit test
bool verifyMd5Sum(const uint8_t* data, size_t dataLen, const String& expectedMd5) {
    // Use the stub in test/stubs/mbedtls.h
    extern int mbedtls_md5(const unsigned char*, size_t, unsigned char*);
    if (data == nullptr || dataLen == 0 || expectedMd5.length() == 0) {
        return false;
    }
    unsigned char hash[16];
    mbedtls_md5(data, dataLen, hash);
    char calculatedMd5[33];
    for (int i = 0; i < 16; i++) {
        sprintf(&calculatedMd5[i * 2], "%02x", hash[i]);
    }
    calculatedMd5[32] = '\0';
    return (expectedMd5.equalsIgnoreCase(calculatedMd5));
}
#endif

void reportFirmwareVersion(CayenneLPP& lpp) {
    // Use CayenneLPP generic sensor to report firmware version as float
    lpp.addGenericSensor(10, FIRMWARE_VERSION_FLOAT);
}

#ifndef UNIT_TEST
bool testWifiConnection(const String& ssid, const String& password) {
    if (ssid.length() == 0) {
        return false;
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Try to connect for 10 seconds
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    bool connected = (WiFi.status() == WL_CONNECTED);
    
    if (connected) {
        Serial.println("WiFi test connection successful");
        Serial.print("IP: "); Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi test connection failed");
    }
    
    // Disconnect for testing
    WiFi.disconnect();
    
    return connected;
}
#else
// Stub for unit test
bool testWifiConnection(const String&, const String&) {
    return true;
}
#endif 