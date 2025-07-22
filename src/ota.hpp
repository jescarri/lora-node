#ifndef OTA_HPP_
#define OTA_HPP_

constexpr int OTA_MAX_CHUNKS      = 20;                                     // Increased for larger payloads
constexpr int OTA_CHUNK_SIZE      = 70;                                     // Increased to accommodate base64 encoded payloads
constexpr int OTA_MAX_BUFFER_SIZE = OTA_MAX_CHUNKS * OTA_CHUNK_SIZE;        // Total buffer size

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.hpp"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <CayenneLPP.h>

// OTA Parse Result enum
enum OtaParseResult {
    OTA_PARSE_OK,
    OTA_PARSE_INCOMPLETE,
    OTA_PARSE_INVALID
};

// OTA Update structure
struct OtaUpdateInfo {
    String url;
    String md5sum;
    String version;
    String signature;        // cryptographic signature (base64)
    bool valid;
};

// OTA state management
extern volatile bool ota_in_progress;

// New OTA chunk buffer - stores raw bytes
struct OtaChunkBuffer {
    uint8_t decoded_chunks[OTA_MAX_CHUNKS][OTA_CHUNK_SIZE];
    int chunk_lens[OTA_MAX_CHUNKS];
    bool received[OTA_MAX_CHUNKS];
    int max_chunk_seen = 0;
    void reset() {
        memset(decoded_chunks, 0, sizeof(decoded_chunks));
        memset(chunk_lens, 0, sizeof(chunk_lens));
        memset(received, 0, sizeof(received));
        max_chunk_seen = 0;
    }
    bool addChunk(int chunk_index, const uint8_t* data, int data_len);
    bool isComplete() const;
    String getJsonString() const;
};

extern OtaChunkBuffer ota_chunk_buffer;

// Function declarations
void handleDownlinkMessage(uint8_t* data, uint8_t dataLen, uint8_t fport);
bool downloadAndInstallFirmware(const OtaUpdateInfo& updateInfo);
bool verifyMd5Sum(const uint8_t* data, size_t dataLen, const String& expectedMd5);
void reportFirmwareVersion(CayenneLPP& lpp);
bool testWifiConnection(const String& ssid, const String& password);
bool verify_signature(const String& url, const String& md5sum, const String& signature_b64);

// OTA state management functions
void setOtaInProgress(bool inProgress);
bool isOtaInProgress();

// Firmware version reporting - will be resolved at runtime
int getFirmwareVersionInt();

// New OTA chunk handling functions
void handleOtaChunk(uint8_t* data, uint8_t dataLen, uint8_t fport);

#endif        // OTA_HPP_
