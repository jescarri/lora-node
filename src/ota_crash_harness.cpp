#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <esp_task_wdt.h>
#include <esp_heap_caps.h>
#include <mbedtls/md5.h>

// Configuration - replace with your actual values
// IMPORTANT: You must update these before uploading!
const char* ssid     = "pastor_tacos";               // <-- CHANGE THIS
const char* password = "concebollaycilantro";        // <-- CHANGE THIS
// Use a URL to a large file (20MB) for testing - you can create one with:
// dd if=/dev/urandom of=test_20mb.bin bs=1M count=20
// Or use any large file URL (firmware, video, etc.)
const char* firmware_url = "https://tinyurl.com/28tsogdh";        // ~20MB ESP32 release

void printMemoryInfo(const char* label) {
    Serial.printf("\n=== Memory Info: %s ===\n", label);
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Min free heap: %d bytes\n", ESP.getMinFreeHeap());
    Serial.printf("Largest free block: %d bytes\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    Serial.printf("Free internal: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    Serial.printf("Free DMA: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_DMA));
    Serial.printf("======================\n");
}

bool crashHarnessDownloadAndInstall(const String& url) {
    printMemoryInfo("Before download start");

    HTTPClient http;
    http.begin(url);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setRedirectLimit(10);

    Serial.printf("Starting download from: %s\n", url.c_str());
    int httpCode = http.GET();
    Serial.printf("HTTP response code: %d\n", httpCode);

    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("HTTP GET failed: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }

    // THIS IS THE CRITICAL PART - Check http.getSize() behavior
    int contentLength = http.getSize();
    Serial.printf("Content length from http.getSize(): %d bytes\n", contentLength);
    Serial.printf("Content length in MB: %.2f MB\n", contentLength / 1024.0 / 1024.0);

    // Check if contentLength is valid
    if (contentLength <= 0) {
        Serial.println("ERROR: Invalid content length!");
        http.end();
        return false;
    }

    // Check if we have enough heap memory for the vector
    size_t free_heap     = ESP.getFreeHeap();
    size_t largest_block = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);

    Serial.printf("Required memory: %d bytes\n", contentLength);
    Serial.printf("Available heap: %zu bytes\n", free_heap);
    Serial.printf("Largest contiguous block: %zu bytes\n", largest_block);

    if (contentLength > largest_block) {
        Serial.println("ERROR: Not enough contiguous memory for firmware buffer!");
        Serial.printf("Need: %d bytes, Available: %zu bytes\n", contentLength, largest_block);
        http.end();
        return false;
    }

    // PROBLEM 1: Using std::vector with reserve() but then insert()
    // reserve() only allocates capacity, doesn't change size
    // insert() at end can cause multiple reallocations
    Serial.println("Creating firmware buffer...");

    std::vector<uint8_t> fw_buf;
    try {
        // This is problematic - reserve doesn't guarantee contiguous allocation
        fw_buf.reserve(contentLength);
        Serial.printf("Buffer reserved, capacity: %zu\n", fw_buf.capacity());
        printMemoryInfo("After reserve()");
    } catch (const std::bad_alloc& e) {
        Serial.println("ERROR: Failed to reserve memory for firmware buffer!");
        http.end();
        return false;
    }

    WiFiClient* stream           = http.getStreamPtr();
    int total_read               = 0;
    uint32_t last_watchdog_reset = millis();

    Serial.println("Starting download...");

    while (total_read < contentLength) {
        // PROBLEM 2: Small buffer size causes many system calls
        uint8_t buf[512];        // This is quite small for large downloads
        int to_read = std::min(512, contentLength - total_read);
        int n       = stream->read(buf, to_read);

        if (n <= 0) {
            Serial.printf("Stream read failed at byte %d\n", total_read);
            break;
        }

        // PROBLEM 3: insert() at end can cause vector reallocation
        // This is extremely memory-intensive for large files
        try {
            fw_buf.insert(fw_buf.end(), buf, buf + n);
        } catch (const std::bad_alloc& e) {
            Serial.printf("ERROR: Memory allocation failed at %d bytes downloaded\n", total_read);
            printMemoryInfo("After allocation failure");
            http.end();
            return false;
        }

        total_read += n;

        // Reset watchdog frequently to prevent timeout
        uint32_t now = millis();
        if (now - last_watchdog_reset >= 1000) {        // Every second
            esp_task_wdt_reset();
            last_watchdog_reset = now;

            // Print progress
            float progress = (total_read * 100.0) / contentLength;
            Serial.printf("Downloaded: %.1f%% (%d/%d bytes)\n",
                          progress, total_read, contentLength);

            // Monitor memory during download
            if (total_read % 102400 == 0) {        // Every 100KB
                printMemoryInfo("During download");
            }
        }
    }

    http.end();

    Serial.printf("Download complete: %d bytes\n", (int)fw_buf.size());
    printMemoryInfo("After download complete");

    // PROBLEM 4: Now we have the entire firmware in RAM AND need to hash it
    // This doubles the memory pressure temporarily
    Serial.println("Calculating MD5...");
    unsigned char hash[16];
    mbedtls_md5(fw_buf.data(), fw_buf.size(), hash);

    char md5str[33];
    for (int i = 0; i < 16; ++i) {
        sprintf(md5str + i * 2, "%02x", hash[i]);
    }
    md5str[32] = 0;
    Serial.printf("Calculated MD5: %s\n", md5str);

    // PROBLEM 5: Update.begin() needs additional memory for flash operations
    Serial.println("Starting OTA update...");
    if (!Update.begin(fw_buf.size())) {
        Serial.println("Update.begin() failed - not enough space");
        Serial.printf("Update error: %s\n", Update.errorString());
        return false;
    }

    printMemoryInfo("After Update.begin()");

    // PROBLEM 6: Update.write() with entire buffer at once
    Serial.println("Writing firmware to flash...");
    size_t written = Update.write(fw_buf.data(), fw_buf.size());

    if (written != fw_buf.size()) {
        Serial.printf("Write failed: %zu of %zu bytes written\n", written, fw_buf.size());
        return false;
    }

    if (!Update.end()) {
        Serial.println("Update.end() failed");
        Serial.printf("Update error: %s\n", Update.errorString());
        return false;
    }

    Serial.println("OTA update completed successfully!");
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("======================================");
    Serial.println("ESP32 OTA Memory Crash Harness");
    Serial.println("Testing downloadAndInstallFirmware");
    Serial.println("======================================");

    printMemoryInfo("Initial");

    // Configure task watchdog with longer timeout for large downloads
    esp_task_wdt_init(10, true);        // 10 second timeout
    esp_task_wdt_add(NULL);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.printf("Connecting to WiFi: %s", ssid);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
        esp_task_wdt_reset();
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nWiFi connection failed!");
        while (true) {
            delay(1000);
            esp_task_wdt_reset();
        }
    }

    Serial.println("\nWiFi connected!");
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());

    delay(2000);

    // Start the problematic download
    Serial.println("Starting OTA crash test...");
    bool result = crashHarnessDownloadAndInstall(firmware_url);

    if (result) {
        Serial.println("Test completed successfully - restarting...");
        delay(3000);
        ESP.restart();
    } else {
        Serial.println("Test failed - examining crash...");
    }
}

void loop() {
    esp_task_wdt_reset();
    delay(1000);
    Serial.println("Test completed - in loop");
}
