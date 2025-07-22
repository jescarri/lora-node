#include "ota.hpp"
#include "lorawan_settings.hpp"
#include "menu.hpp"
#include "utils.hpp"
#include "version.hpp"
#include "debug.hpp"
#include <sodium.h>
#include <esp_task_wdt.h>

// OTA state
volatile bool ota_in_progress = false;
OtaChunkBuffer ota_chunk_buffer;

// Update handleOtaChunk to use ota_chunk_buffer.addChunk, isComplete, and getJsonString
void handleOtaChunk(uint8_t* data, uint8_t dataLen, uint8_t fport) {
    int idx = fport - 1;
    DEBUG_PRINTF("[OTA] handleOtaChunk: fport=%d idx=%d dataLen=%d\r\n", fport, idx, dataLen);
    DEBUG_PRINT("[OTA] Chunk data: ");
    for (int i = 0; i < dataLen; ++i) DEBUG_PRINTF("%02X ", data[i]);
    DEBUG_PRINTF("\r\n");
    if (idx < 0 || idx >= OTA_MAX_CHUNKS) {
        DEBUG_PRINTF("[OTA] Invalid chunk index: %d\r\n", idx);
        return;
    }
    if (!ota_chunk_buffer.addChunk(idx, data, dataLen)) {
        DEBUG_PRINTF("[OTA] Failed to add chunk idx=%d len=%d\r\n", idx, dataLen);
        return;
    }
    DEBUG_PRINTF("[OTA] Chunk %d added. Checking completeness...\r\n", idx);
    if (ota_chunk_buffer.isComplete()) {
        DEBUG_PRINTLN("[OTA] All chunks received. Attempting reassembly and JSON parse...");
        String json = ota_chunk_buffer.getJsonString();
        DEBUG_PRINTF("[OTA] Reassembled JSON (%d bytes): %s\r\n", json.length(), json.c_str());
        JsonDocument doc;
        auto error = deserializeJson(doc, json);
        if (!error) {
            Serial.println("[OTA] JSON parsed successfully. Triggering firmware update.");
            OtaUpdateInfo updateInfo;
            // Extract fields (support both full and short names)
            if (doc["url"]) {
                updateInfo.url = doc["url"].as<String>();
            } else if (doc["u"]) {
                updateInfo.url = doc["u"].as<String>();
            } else {
                Serial.println("Missing url field");
                ota_chunk_buffer.reset();
                return;
            }
            if (doc["md5sum"]) {
                updateInfo.md5sum = doc["md5sum"].as<String>();
            } else if (doc["m"]) {
                updateInfo.md5sum = doc["m"].as<String>();
            } else {
                Serial.println("Missing md5sum field");
                ota_chunk_buffer.reset();
                return;
            }
            if (doc["version"]) {
                updateInfo.version = doc["version"].as<String>();
            } else if (doc["v"]) {
                updateInfo.version = doc["v"].as<String>();
            } else {
                Serial.println("Missing version field");
                ota_chunk_buffer.reset();
                return;
            }
            if (doc["signature"]) {
                updateInfo.signature = doc["signature"].as<String>();
            } else if (doc["s"]) {
                updateInfo.signature = doc["s"].as<String>();
            } else {
                Serial.println("Missing signature field");
                ota_chunk_buffer.reset();
                return;
            }
            updateInfo.valid = true;
            setOtaInProgress(true);
            if (verify_signature(updateInfo.url, updateInfo.md5sum, updateInfo.signature)) {
                Serial.println("Signature verification successful");
                if (downloadAndInstallFirmware(updateInfo)) {
                    Serial.println("Firmware update completed successfully - rebooting");
                    ota_chunk_buffer.reset();
                    setOtaInProgress(false);
                    delay(1000);
                    ESP.restart();
                } else {
                    Serial.println("Firmware update failed");
                    setOtaInProgress(false);
                    ota_chunk_buffer.reset();
                }
            } else {
                Serial.println("Signature verification failed");
                setOtaInProgress(false);
                ota_chunk_buffer.reset();
            }
        } else {
            DEBUG_PRINTF("[OTA] JSON parse failed: %s\r\n", error.c_str());
        }
        // else: wait for more chunks or reset on fatal error
    } else {
        DEBUG_PRINTLN("[OTA] Waiting for more chunks...");
    }
}

void handleDownlinkMessage(uint8_t* data, uint8_t dataLen, uint8_t fport) {
    Serial.printf("Downlink received: fport=%d, len=%d\r\n", fport, dataLen);

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
    return verify_signature(updateInfo.url, updateInfo.md5sum, updateInfo.signature);
}

// Legacy parse function with result
OtaParseResult parseOtaMessageWithResult(const uint8_t* data, uint8_t dataLen, OtaUpdateInfo& updateInfo) {
    if (parseOtaMessage(data, dataLen, updateInfo)) {
        return OTA_PARSE_OK;
    }
    return OTA_PARSE_INVALID;
}

// Move base64 helpers above verify_signature
static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int base64_dec_len(const char* input, int length) {
    int padding = 0;
    if (length > 0 && input[length - 1] == '=') padding++;
    if (length > 1 && input[length - 2] == '=') padding++;
    return (length * 3) / 4 - padding;
}

static bool base64_decode(unsigned char* output, const char* input, int length) {
    int i = 0, j = 0;
    int a, b, c, d;
    while (i < length) {
        // Handle padding and end of string
        if (input[i] == '=' || input[i] == 0) break;
        
        // Find index of first character
        for (a = 0; a < 64 && base64_chars[a] != input[i]; a++);
        if (a == 64 || ++i >= length) return false;
        
        // Find index of second character
        if (input[i] == '=' || input[i] == 0) return false;
        for (b = 0; b < 64 && base64_chars[b] != input[i]; b++);
        if (b == 64 || ++i >= length) return false;
        
        // Decode first byte
        output[j++] = (a << 2) | (b >> 4);
        
        // Handle third character (might be padding)
        if (input[i] == '=' || input[i] == 0) break;
        for (c = 0; c < 64 && base64_chars[c] != input[i]; c++);
        if (c == 64) {
            if (input[i] != '=') return false;
            break;
        }
        i++;
        
        // Decode second byte
        output[j++] = (b << 4) | (c >> 2);
        
        // Handle fourth character (might be padding)
        if (i >= length || input[i] == '=' || input[i] == 0) break;
        for (d = 0; d < 64 && base64_chars[d] != input[i]; d++);
        if (d == 64) {
            if (input[i] != '=') return false;
            break;
        }
        i++;
        
        // Decode third byte
        output[j++] = (c << 6) | d;
    }
    return true;
}

bool verify_signature(const String& url, const String& md5sum, const String& signature_b64) {
    if (url.length() == 0 || md5sum.length() == 0 || signature_b64.length() == 0) {
        Serial.println("Empty url, md5sum, or signature");
        return false;
    }
    String message = url + md5sum;
    Serial.printf("Verifying signature for message: %s\r\n", message.c_str());
    Serial.printf("Signature (base64): %s\r\n", signature_b64.c_str());
    // Get public key from build flag
    const char* pubkey_hex = IDENTITYLABS_PUB_KEY;
    Serial.printf("Public key: %s\r\n", pubkey_hex);
    // Convert hex public key to bytes
    unsigned char pubkey[32];
    if (strlen(pubkey_hex) != 64) {
        Serial.println("Invalid public key length");
        return false;
    }
    for (int i = 0; i < 32; i++) {
        char hex_byte[3] = {pubkey_hex[i * 2], pubkey_hex[i * 2 + 1], 0};
        pubkey[i]        = strtol(hex_byte, NULL, 16);
    }
    // Decode base64 signature
    int sig_len = base64_dec_len(signature_b64.c_str(), signature_b64.length());
    if (sig_len != 64) {
        Serial.printf("Invalid signature length: %d\r\n", sig_len);
        return false;
    }
    unsigned char sig[64];
    base64_decode(sig, signature_b64.c_str(), signature_b64.length());
    // Verify signature using libsodium
    if (crypto_sign_verify_detached(sig, (const unsigned char*)message.c_str(), message.length(), pubkey) != 0) {
        Serial.println("Signature verification failed");
        return false;
    }
    Serial.println("Signature verification successful");
    return true;
}

#include <mbedtls/md5.h>
bool downloadAndInstallFirmware(const OtaUpdateInfo& updateInfo) {
    if (!updateInfo.valid) {
        Serial.println("Invalid update info");
        return false;
    }
    // Get saved WiFi credentials
    String ssid     = settings_get_string("wifi_ssid");
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
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    // Download firmware
    HTTPClient http;
    http.begin(updateInfo.url);
    
    // Enable following redirects (important for URL shorteners like TinyURL)
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setRedirectLimit(10); // Allow up to 10 redirects
    
    Serial.printf("Starting download from: %s\r\n", updateInfo.url.c_str());
    int httpCode = http.GET();
    
    Serial.printf("HTTP response code: %d\r\n", httpCode);
    
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("HTTP GET failed, error: %s\r\n", http.errorToString(httpCode).c_str());
        
        // Check for redirect codes that should have been handled
        if (httpCode == 301 || httpCode == 302 || httpCode == 307 || httpCode == 308) {
            String location = http.header("Location");
            Serial.printf("Redirect location: %s\r\n", location.c_str());
        }
        
        http.end();
        return false;
    }
    int contentLength = http.getSize();
    Serial.printf("Content length: %d\r\n", contentLength);
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
    // Download firmware to buffer for MD5 validation
    std::vector<uint8_t> fw_buf;
    fw_buf.reserve(contentLength);
    WiFiClient* stream = http.getStreamPtr();
    int total_read     = 0;
    while (total_read < contentLength) {
        uint8_t buf[512];
        int to_read = std::min(512, contentLength - total_read);
        int n       = stream->read(buf, to_read);
        if (n <= 0) break;
        fw_buf.insert(fw_buf.end(), buf, buf + n);
        total_read += n;
        
        // Reset watchdog every 512 bytes to prevent timeout
        esp_task_wdt_reset();
        
        // Print progress every 10KB
        if (total_read % 10240 == 0) {
            Serial.printf("Downloaded %d/%d bytes\r\n", total_read, contentLength);
        }
    }
    http.end();
    Serial.printf("Downloaded %d bytes\r\n", (int)fw_buf.size());
    // Calculate MD5
    unsigned char hash[16];
    mbedtls_md5(fw_buf.data(), fw_buf.size(), hash);
    char md5str[33];
    for (int i = 0; i < 16; ++i) sprintf(md5str + i * 2, "%02x", hash[i]);
    md5str[32] = 0;
    Serial.printf("Calculated MD5: %s\r\n", md5str);
    Serial.printf("Expected MD5: %s\r\n", updateInfo.md5sum.c_str());
    if (strcasecmp(md5str, updateInfo.md5sum.c_str()) != 0) {
        Serial.println("MD5 mismatch! Aborting update.");
        return false;
    }
    Serial.println("MD5 matches. Proceeding with OTA update.");
    // Start update
    if (!Update.begin(fw_buf.size())) {
        Serial.println("Not enough space to begin OTA");
        return false;
    }
    size_t written = Update.write(fw_buf.data(), fw_buf.size());
    if (written != fw_buf.size()) {
        Serial.printf("Written %d of %d bytes\r\n", (int)written, (int)fw_buf.size());
        return false;
    }
    if (!Update.end()) {
        Serial.println("Update end failed");
        return false;
    }
    Serial.println("OTA update completed successfully");
    return true;
}

bool verifyMd5Sum(const uint8_t* data, size_t dataLen, const String& expectedMd5) {
    if (data == nullptr || dataLen == 0 || expectedMd5.length() == 0) {
        return false;
    }

    unsigned char hash[16];
    mbedtls_md5(data, dataLen, hash);

    String calculatedMd5 = "";
    for (int i = 0; i < 16; i++) {
        if (hash[i] < 0x10) {
            calculatedMd5 += "0";
        }
        calculatedMd5 += String(hash[i], HEX);
    }

    Serial.print("Calculated MD5: ");
    Serial.println(calculatedMd5);
    Serial.print("Expected MD5: ");
    Serial.println(expectedMd5);

    return calculatedMd5.equalsIgnoreCase(expectedMd5);
}

void reportFirmwareVersion(CayenneLPP& lpp) {
    // Use CayenneLPP generic sensor to report firmware version as integer
    // Send the 3-digit version directly (e.g., 110 for v1.1.0)
    int versionInt = version::getFirmwareVersionInt();
    lpp.addGenericSensor(10, static_cast<float>(versionInt));
    Serial.printf("Reporting firmware version: %d\r\n", versionInt);
}

int getFirmwareVersionInt() {
    return version::getFirmwareVersionInt();
}

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
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi test connection failed");
    }

    // Disconnect for testing
    WiFi.disconnect();

    return connected;
}

void setOtaInProgress(bool inProgress) {
    ota_in_progress = inProgress;
    Serial.printf("OTA in progress: %s\r\n", inProgress ? "true" : "false");
}

bool isOtaInProgress() {
    return ota_in_progress;
}

// --- OtaChunkBuffer methods for chunked OTA reassembly ---
bool OtaChunkBuffer::addChunk(int chunk_index, const uint8_t* data, int data_len) {
    if (chunk_index < 0 || chunk_index >= OTA_MAX_CHUNKS) return false;
    if (data_len > OTA_CHUNK_SIZE) return false;
    memcpy(decoded_chunks[chunk_index], data, data_len);
    chunk_lens[chunk_index] = data_len;
    received[chunk_index]   = true;
    if (chunk_index > max_chunk_seen) max_chunk_seen = chunk_index;
    return true;
}

bool OtaChunkBuffer::isComplete() const {
    for (int i = 0; i <= max_chunk_seen; ++i) {
        if (!received[i]) return false;
    }
    return true;
}

String OtaChunkBuffer::getJsonString() const {
    String out;
    for (int i = 0; i <= max_chunk_seen; ++i) {
        out += String((const char*)decoded_chunks[i], chunk_lens[i]);
    }
    return out;
}
