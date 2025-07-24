#include "ota.hpp"
#include "lorawan_settings.hpp"
#include "lorawan.hpp"
#include "menu.hpp"
#include "utils.hpp"
#include "version.hpp"
#include <sodium.h>
#include <esp_task_wdt.h>
#include <lmic.h>
#include <esp_wifi.h>
#include <esp_event.h>

// OTA state
volatile bool ota_in_progress = false;
OtaChunkBuffer ota_chunk_buffer;

// Update handleOtaChunk to use ota_chunk_buffer.addChunk, isComplete, and getJsonString
void handleOtaChunk(uint8_t* data, uint8_t dataLen, uint8_t fport) {
    int idx = fport - 1;
    if (idx < 0 || idx >= OTA_MAX_CHUNKS) {
        return;
    }

    // Check if this is a potential new OTA session by detecting out-of-order delivery
    // If we receive any chunk but no previous chunks, reset the buffer
    if (ota_chunk_buffer.max_chunk_seen == 0 && !ota_chunk_buffer.received[0] && fport != 1) {
        Serial.printf("[DEBUG] Out-of-order chunk delivery detected (fport=%d) - resetting buffer\r\n", fport);
        ota_chunk_buffer.reset();
    }

    // Always reset buffer when receiving the first chunk (fport=1) to start fresh OTA session
    if (fport == 1) {
        Serial.println("[DEBUG] Starting new OTA session - resetting chunk buffer");
        ota_chunk_buffer.reset();
    }

    if (!ota_chunk_buffer.addChunk(idx, data, dataLen)) {
        return;
    }
    if (ota_chunk_buffer.isComplete()) {
        String json = ota_chunk_buffer.getJsonString();
        JsonDocument doc;
        auto error = deserializeJson(doc, json);
        if (!error) {
            Serial.println("[OTA] JSON parsed successfully. Triggering firmware update.");
            Serial.printf("[DEBUG] Raw JSON string: %s\r\n", json.c_str());
            Serial.printf("[DEBUG] JSON length: %d\r\n", json.length());

            OtaUpdateInfo updateInfo;
            // Extract fields (support both full and short names)
            if (doc["url"]) {
                updateInfo.url = doc["url"].as<String>();
                Serial.printf("[DEBUG] Extracted URL (from 'url'): %s\r\n", updateInfo.url.c_str());
            } else if (doc["u"]) {
                updateInfo.url = doc["u"].as<String>();
                Serial.printf("[DEBUG] Extracted URL (from 'u'): %s\r\n", updateInfo.url.c_str());
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
        }
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

bool downloadAndInstallFirmware(const OtaUpdateInfo& updateInfo) {
    if (!updateInfo.valid) {
        Serial.println("Invalid update info");
        return false;
    }
    delay(100);
    setCpuFrequencyMhz(240);
    Serial.updateBaudRate(115200);
    delay(100);
    Serial.println("Starting WiFi setup for OTA download...");

    // Reset watchdog before starting OTA operations
    esp_task_wdt_reset();

    // Get saved WiFi credentials
    String ssid     = settings_get_string("wifi_ssid");
    String password = settings_get_string("wifi_password");

    Serial.printf("SSID from settings: '%s' (length: %d)\r\n", ssid.c_str(), ssid.length());
    Serial.printf("Password from settings: '%s' (length: %d)\r\n",
                  password.length() > 0 ? "[CONFIGURED]" : "[EMPTY]", password.length());

    if (ssid.length() == 0) {
        Serial.println("ERROR: No WiFi SSID configured in settings!");
        return false;
    }

    // Disconnect any existing WiFi connections and reset state
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_OFF);
    delay(100);

    // Enable WiFi for download
    Serial.println("Enabling WiFi in STA mode...");
    WiFi.mode(WIFI_STA);
    delay(100);

    Serial.printf("Connecting to WiFi SSID: %s\r\n", ssid.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());

    bool connected                 = false;
    unsigned long startAttemptTime = millis();
    Serial.print("Connecting to WiFi...");

    while (!connected && (millis() - startAttemptTime < 8000)) {
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
        }
        delay(100);
        esp_task_wdt_reset();
    }

    if (!connected) {
        Serial.println("\nERROR: Failed to connect to WiFi within timeout!");
        Serial.printf("Final WiFi status: %d\n", WiFi.status());
        return false;
    }

    Serial.println("\nSUCCESS: WiFi connected!");
    Serial.printf("IP Address: %s\r\n", WiFi.localIP().toString().c_str());
    Serial.printf("Gateway: %s\r\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("DNS: %s\r\n", WiFi.dnsIP().toString().c_str());
    Serial.printf("Signal strength: %d dBm\r\n", WiFi.RSSI());

    // Configure HTTPUpdate with callbacks for progress monitoring
    httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    httpUpdate.rebootOnUpdate(false);  // We'll handle reboot ourselves
    
    // Set up progress callback
    httpUpdate.onProgress([](int progress, int total) {
        static unsigned long lastUpdate = 0;
        unsigned long now = millis();
        
        // Update every 2 seconds to reduce serial spam
        if (now - lastUpdate >= 2000 || progress == total) {
            Serial.printf("OTA Progress: %d/%d bytes (%.1f%%)\r\n", 
                         progress, total, (progress * 100.0) / total);
            lastUpdate = now;
        }
        
        // Reset watchdog during download
        esp_task_wdt_reset();
    });
    
    // Set up error callback
    httpUpdate.onError([](int error) {
        Serial.printf("HTTPUpdate error: %d - %s\r\n", error, httpUpdate.getLastErrorString().c_str());
    });
    
    // Set up start callback
    httpUpdate.onStart([]() {
        Serial.println("HTTPUpdate started");
    });
    
    // Set up end callback
    httpUpdate.onEnd([]() {
        Serial.println("HTTPUpdate finished");
    });

    // Create HTTPClient for the update
    HTTPClient httpClient;
    WiFiClientSecure* secureClient = nullptr;
    
    Serial.printf("Starting OTA download from: %s\r\n", updateInfo.url.c_str());
    
    // Determine if we need HTTPS support
    if (updateInfo.url.startsWith("https://")) {
        secureClient = new WiFiClientSecure();
        secureClient->setInsecure(); // Skip certificate validation for simplicity
        if (!httpClient.begin(*secureClient, updateInfo.url)) {
            Serial.println("HTTPClient begin failed for HTTPS URL");
            delete secureClient;
            return false;
        }
    } else {
        if (!httpClient.begin(updateInfo.url)) {
            Serial.println("HTTPClient begin failed for HTTP URL");
            return false;
        }
    }
    
    // Configure HTTP timeouts
    httpClient.setTimeout(30000);      // 30 second timeout
    httpClient.setConnectTimeout(15000); // 15 second connection timeout
    
    // Add custom header with expected MD5 for verification
    httpClient.addHeader("x-MD5", updateInfo.md5sum);
    
    Serial.printf("Expected MD5: %s\r\n", updateInfo.md5sum.c_str());
    
    // Perform the update
    HTTPUpdateResult result = httpUpdate.update(httpClient, updateInfo.version);
    
    // Clean up HTTPS client if created
    if (secureClient) {
        delete secureClient;
    }
    
    switch (result) {
        case HTTP_UPDATE_OK:
            Serial.println("OTA update completed successfully!");
            Serial.println("Firmware updated, ready to reboot");
            esp_task_wdt_reset();
            return true;
            
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("No updates available (same version)");
            return false;
            
        case HTTP_UPDATE_FAILED:
        default:
            Serial.printf("OTA update failed. Error (%d): %s\r\n", 
                         httpUpdate.getLastError(), 
                         httpUpdate.getLastErrorString().c_str());
            return false;
    }
}

bool verifyMd5Sum(const uint8_t* data, size_t dataLen, const String& expectedMd5) {
    if (data == nullptr || dataLen == 0 || expectedMd5.length() == 0) {
        return false;
    }

    MD5Builder md5;
    md5.begin();
    md5.add(const_cast<uint8_t*>(data), dataLen);
    md5.calculate();
    String calculatedMd5 = md5.toString();

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

    // Non-blocking connection with 8 second timeout
    bool connected             = false;
    unsigned long startTime    = millis();
    unsigned long lastWdtReset = millis();

    Serial.print("Testing WiFi connection...");

    while (!connected && (millis() - startTime < 8000)) {
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
        } else {
            delay(100);

            // Reset watchdog every 400ms
            if (millis() - lastWdtReset >= 400) {
                esp_task_wdt_reset();
                lastWdtReset = millis();
                Serial.print(".");
            }
        }
    }

    if (connected) {
        Serial.println("\nWiFi test connection successful");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi test connection failed");
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
    if (chunk_index < 0 || chunk_index >= OTA_MAX_CHUNKS) {
        Serial.printf("[DEBUG] Chunk index %d out of range (0-%d)\r\n", chunk_index, OTA_MAX_CHUNKS - 1);
        return false;
    }
    if (data_len > OTA_CHUNK_SIZE) {
        Serial.printf("[DEBUG] Chunk data length %d exceeds max %d\r\n", data_len, OTA_CHUNK_SIZE);
        return false;
    }
    memcpy(decoded_chunks[chunk_index], data, data_len);
    chunk_lens[chunk_index] = data_len;
    received[chunk_index]   = true;
    if (chunk_index > max_chunk_seen) max_chunk_seen = chunk_index;

    Serial.printf("[DEBUG] Added chunk %d: %d bytes (max_chunk_seen=%d)\r\n", chunk_index, data_len, max_chunk_seen);

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
