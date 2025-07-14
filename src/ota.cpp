#include "ota.hpp"
#include "lorawan_settings.hpp"
#include "menu.hpp"
#include "utils.hpp"
#include "version.hpp"
#ifndef UNIT_TEST
#include <sodium.h>
#endif

// OTA state
volatile bool ota_in_progress = false;
OtaChunkBuffer ota_chunk_buffer;

// Simple base64 decoder
static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int base64_dec_len(const char* input, int length) {
    int padding = 0;
    if (length > 0 && input[length - 1] == '=') padding++;
    if (length > 1 && input[length - 2] == '=') padding++;
    return (length * 3) / 4 - padding;
}

static bool base64_decode(unsigned char* output, const char* input, int length) {
    int i = 0, j = 0;
    int dec_len = 0;
    int a, b, c, d;
    
    while (i < length) {
        // Find the base64 character
        for (a = 0; a < 64 && base64_chars[a] != input[i]; a++);
        i++;
        if (a == 64) return false;
        
        for (b = 0; b < 64 && base64_chars[b] != input[i]; b++);
        i++;
        if (b == 64) return false;
        
        for (c = 0; c < 64 && base64_chars[c] != input[i]; c++);
        i++;
        if (c == 64) return false;
        
        for (d = 0; d < 64 && base64_chars[d] != input[i]; d++);
        i++;
        if (d == 64) return false;
        
        output[j++] = (a << 2) | (b >> 4);
        output[j++] = (b << 4) | (c >> 2);
        output[j++] = (c << 6) | d;
    }
    
    return true;
}

// New OTA chunk handling - simplified approach
void handleOtaChunk(uint8_t* data, uint8_t dataLen, uint8_t fport) {
    if (data == nullptr || dataLen == 0) {
        Serial.println("handleOtaChunk: Invalid data");
        return;
    }
    
    // fport is 1-based chunk index
    int chunk_index = fport - 1;
    
    Serial.printf("Received OTA chunk %d, length=%d\n", fport, dataLen);
    
    // Add chunk to buffer
    if (!ota_chunk_buffer.addChunk(chunk_index, data, dataLen)) {
        Serial.printf("Failed to add chunk %d to buffer\n", fport);
        return;
    }
    
    Serial.printf("Added chunk %d, total_bytes=%d, max_chunk=%d\n", 
                  fport, ota_chunk_buffer.total_bytes, ota_chunk_buffer.max_chunk_seen);
    
    // Try to parse JSON after each chunk
    if (processOtaUpdate()) {
        Serial.println("OTA update processed successfully");
    }
}

// Process OTA update - attempts JSON parsing and handles the update
bool processOtaUpdate() {
    if (ota_chunk_buffer.total_bytes == 0) {
        return false;
    }
    
    // Get the JSON string from buffer
    String jsonString = ota_chunk_buffer.getJsonString();
    Serial.printf("Attempting to parse JSON (%d bytes): %s\n", 
                  jsonString.length(), jsonString.c_str());
    
    // Try to parse JSON
    JsonDocument doc;
    auto error = deserializeJson(doc, jsonString);
    
    if (error) {
        Serial.printf("JSON parsing failed: %s\n", error.c_str());
        
        // Check if it's incomplete input
        if (String(error.c_str()).indexOf("IncompleteInput") >= 0) {
            Serial.println("Incomplete JSON - waiting for more chunks");
            return false; // Wait for more chunks
        }
        
        // Invalid JSON - reset buffer
        Serial.println("Invalid JSON - resetting buffer");
        ota_chunk_buffer.reset();
        return false;
    }
    
    // JSON parsed successfully - extract OTA info
    Serial.println("JSON parsed successfully!");
    
    OtaUpdateInfo updateInfo = {};
    
    // Extract fields (support both full and short names)
    if (doc.containsKey("url")) {
        updateInfo.url = doc["url"].as<String>();
    } else if (doc.containsKey("u")) {
        updateInfo.url = doc["u"].as<String>();
    } else {
        Serial.println("Missing url field");
        ota_chunk_buffer.reset();
        return false;
    }
    
    if (doc.containsKey("md5sum")) {
        updateInfo.md5sum = doc["md5sum"].as<String>();
    } else if (doc.containsKey("m")) {
        updateInfo.md5sum = doc["m"].as<String>();
    } else {
        Serial.println("Missing md5sum field");
        ota_chunk_buffer.reset();
        return false;
    }
    
    if (doc.containsKey("version")) {
        updateInfo.version = doc["version"].as<String>();
    } else if (doc.containsKey("v")) {
        updateInfo.version = doc["v"].as<String>();
    } else {
        Serial.println("Missing version field");
        ota_chunk_buffer.reset();
        return false;
    }
    
    if (doc.containsKey("signature")) {
        updateInfo.signature = doc["signature"].as<String>();
    } else if (doc.containsKey("s")) {
        updateInfo.signature = doc["s"].as<String>();
    } else {
        Serial.println("Missing signature field");
        ota_chunk_buffer.reset();
        return false;
    }
    
    updateInfo.valid = true;
    
    Serial.printf("OTA Info: url=%s, md5=%s, version=%s, sig=%s\n",
                  updateInfo.url.c_str(), updateInfo.md5sum.c_str(), 
                  updateInfo.version.c_str(), updateInfo.signature.c_str());
    
    // Set OTA in progress
    setOtaInProgress(true);
    
    // Verify signature
    if (!verify_signature(updateInfo.md5sum, updateInfo.signature)) {
        Serial.println("Signature verification failed");
        setOtaInProgress(false);
        ota_chunk_buffer.reset();
        return false;
    }
    
    Serial.println("Signature verification successful");
    
    // Download and install firmware
    if (downloadAndInstallFirmware(updateInfo)) {
        Serial.println("Firmware update completed successfully - rebooting");
        ota_chunk_buffer.reset();
        setOtaInProgress(false);
        
        // Reboot after successful update
        delay(1000);
        ESP.restart();
        return true;
    } else {
        Serial.println("Firmware update failed");
        setOtaInProgress(false);
        ota_chunk_buffer.reset();
        return false;
    }
}

void handleDownlinkMessage(uint8_t* data, uint8_t dataLen, uint8_t fport) {
    Serial.printf("Downlink received: fport=%d, len=%d\n", fport, dataLen);
    
    // Handle OTA chunks (fport 1-20)
    if (fport >= 1 && fport <= OTA_MAX_CHUNKS) {
        handleOtaChunk(data, dataLen, fport);
        return;
    }
    
    // Handle other downlink messages
    Serial.println("Non-OTA downlink message");
}

// Legacy parse function for compatibility
bool parseOtaMessage(const uint8_t* data, uint8_t dataLen, OtaUpdateInfo& updateInfo) {
    if (data == nullptr || dataLen == 0) {
        return false;
    }
    
    String jsonString = String(reinterpret_cast<const char*>(data), dataLen);
    JsonDocument doc;
    auto error = deserializeJson(doc, jsonString);
    
    if (error) {
        return false;
    }
    
    // Extract fields
    if (doc.containsKey("url")) {
        updateInfo.url = doc["url"].as<String>();
    } else if (doc.containsKey("u")) {
        updateInfo.url = doc["u"].as<String>();
    } else {
        return false;
    }
    
    if (doc.containsKey("md5sum")) {
        updateInfo.md5sum = doc["md5sum"].as<String>();
    } else if (doc.containsKey("m")) {
        updateInfo.md5sum = doc["m"].as<String>();
    } else {
        return false;
    }
    
    if (doc.containsKey("version")) {
        updateInfo.version = doc["version"].as<String>();
    } else if (doc.containsKey("v")) {
        updateInfo.version = doc["v"].as<String>();
    } else {
        return false;
    }
    
    if (doc.containsKey("signature")) {
        updateInfo.signature = doc["signature"].as<String>();
    } else if (doc.containsKey("s")) {
        updateInfo.signature = doc["s"].as<String>();
    } else {
        return false;
    }
    
    updateInfo.valid = true;
    return verify_signature(updateInfo.md5sum, updateInfo.signature);
}

// Legacy parse function with result
OtaParseResult parseOtaMessageWithResult(const uint8_t* data, uint8_t dataLen, OtaUpdateInfo& updateInfo) {
    if (parseOtaMessage(data, dataLen, updateInfo)) {
        return OTA_PARSE_OK;
    }
    return OTA_PARSE_INVALID;
}

#ifdef UNIT_TEST
bool verify_signature(const std::string& md5sum, const std::string& signature_b64) {
    return md5sum == "goodmd5" && signature_b64 == "VALIDSIG";
}
#else
bool verify_signature(const String& md5sum, const String& signature_b64) {
    if (md5sum.length() == 0 || signature_b64.length() == 0) {
        Serial.println("Empty md5sum or signature");
        return false;
    }
    
    Serial.printf("Verifying signature for md5: %s\n", md5sum.c_str());
    Serial.printf("Signature (base64): %s\n", signature_b64.c_str());
    
    // Get public key from build flag
    const char* pubkey_hex = IDENTITYLABS_PUB_KEY;
    Serial.printf("Public key: %s\n", pubkey_hex);
    
    // Convert hex public key to bytes
    unsigned char pubkey[32];
    if (strlen(pubkey_hex) != 64) {
        Serial.println("Invalid public key length");
        return false;
    }
    
    for (int i = 0; i < 32; i++) {
        char hex_byte[3] = {pubkey_hex[i*2], pubkey_hex[i*2+1], 0};
        pubkey[i] = strtol(hex_byte, NULL, 16);
    }
    
    // Decode base64 signature
    int sig_len = base64_dec_len(signature_b64.c_str(), signature_b64.length());
    if (sig_len != 64) {
        Serial.printf("Invalid signature length: %d\n", sig_len);
        return false;
    }
    
    unsigned char sig[64];
    base64_decode(sig, signature_b64.c_str(), signature_b64.length());
    
    // Verify signature using libsodium
    if (crypto_sign_verify_detached(sig, (const unsigned char*)md5sum.c_str(), md5sum.length(), pubkey) != 0) {
        Serial.println("Signature verification failed");
        return false;
    }
    
    Serial.println("Signature verification successful");
    return true;
}
#endif

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
    if (updateInfo.md5sum.length() > 0 && !Update.setMD5(updateInfo.md5sum.c_str())) {
        Serial.println("Failed to set MD5");
        return false;
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
bool verifyMd5Sum(const uint8_t* data, size_t dataLen, const String& expectedMd5) {
    if (data == nullptr || dataLen == 0 || expectedMd5.length() == 0) {
        return false;
    }

    unsigned char hash[16];
    extern int mbedtls_md5(const unsigned char*, size_t, unsigned char*);
    
    if (mbedtls_md5(data, dataLen, hash) != 0) {
        return false;
    }

    String calculatedMd5 = "";
    for (int i = 0; i < 16; i++) {
        if (hash[i] < 0x10) {
            calculatedMd5 += "0";
        }
        calculatedMd5 += String(hash[i], HEX);
    }

    Serial.print("Calculated MD5: "); Serial.println(calculatedMd5);
    Serial.print("Expected MD5: "); Serial.println(expectedMd5);

    return calculatedMd5.equalsIgnoreCase(expectedMd5);
}
#else
bool verifyMd5Sum(const uint8_t*, size_t, const String&) {
    return true;
}
#endif

void reportFirmwareVersion(CayenneLPP& lpp) {
    // Use CayenneLPP generic sensor to report firmware version as integer
    // Send the 3-digit version directly (e.g., 110 for v1.1.0)
    int versionInt = version::getFirmwareVersionInt();
    lpp.addGenericSensor(10, static_cast<float>(versionInt));
    Serial.printf("Reporting firmware version: %d\n", versionInt);
}

int getFirmwareVersionInt() {
    return version::getFirmwareVersionInt();
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
bool testWifiConnection(const String&, const String&) {
    return true;
}
#endif

void setOtaInProgress(bool inProgress) {
    ota_in_progress = inProgress;
    Serial.printf("OTA in progress: %s\n", inProgress ? "true" : "false");
}

bool isOtaInProgress() {
    return ota_in_progress;
}
