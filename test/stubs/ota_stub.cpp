#include "ota.hpp"
#include <string>
#include <cstring>
#include <iostream>

// Global variables for testing
bool ota_update_available = false;
std::string ota_firmware_url;
std::string ota_firmware_version;
std::string ota_firmware_md5;
bool ota_download_success = false;
bool ota_verify_success = false;
bool ota_apply_success = false;

// Arduino String class stub
class String {
private:
    std::string data;
public:
    String() : data("") {}
    String(const char* str) : data(str ? str : "") {}
    String(const std::string& str) : data(str) {}
    String(int value) : data(std::to_string(value)) {}
    String(float value) : data(std::to_string(value)) {}
    
    const char* c_str() const { return data.c_str(); }
    int length() const { return static_cast<int>(data.length()); }
    bool equals(const String& other) const { return data == other.data; }
    bool equals(const char* str) const { return data == (str ? str : ""); }
    
    String& operator=(const char* str) {
        data = str ? str : "";
        return *this;
    }
    
    String& operator=(const std::string& str) {
        data = str;
        return *this;
    }
    
    String& operator=(int value) {
        data = std::to_string(value);
        return *this;
    }
    
    String& operator=(float value) {
        data = std::to_string(value);
        return *this;
    }
    
    operator const char*() const { return data.c_str(); }
    operator std::string() const { return data; }
};

// HTTPClient stub
class HTTPClient {
public:
    HTTPClient() {}
    bool begin(const String& url) { return true; }
    int GET() { return 200; }
    String getString() { return "mock_firmware_data"; }
    void end() {}
};

// Update stub
class Update {
public:
    static bool begin(size_t size) { return true; }
    static size_t write(const uint8_t* data, size_t len) { return len; }
    static bool end() { return true; }
    static bool setMD5(const char* md5) { return true; }
    static bool hasError() { return false; }
    static void printError(Stream& stream) {}
};

// Stream stub
class Stream {
public:
    virtual void print(const String& str) {}
    virtual void println(const String& str) {}
};

// Serial stub
class SerialClass : public Stream {
public:
    void print(const String& str) override {}
    void println(const String& str) override {}
    void begin(int baud) {}
};

SerialClass Serial;

// WiFi stub
class WiFiClass {
public:
    bool begin(const char* ssid, const char* password) { return true; }
    bool isConnected() { return true; }
    void disconnect() {}
};

WiFiClass WiFi;

// ArduinoJson stub
namespace ArduinoJson {
    class JsonDocument {
    public:
        bool containsKey(const char* key) const { return true; }
        String get(const char* key) const { return "test_value"; }
        int getInt(const char* key) const { return 100; }
        float getFloat(const char* key) const { return 1.0f; }
    };
    
    JsonDocument parse(const String& json) {
        return JsonDocument();
    }
}

// OTA functions
bool parseOtaMessage(const String& payload) {
    if (payload.length() > 0) {
        ota_update_available = true;
        ota_firmware_url = "https://example.com/firmware.bin";
        ota_firmware_version = "100";
        ota_firmware_md5 = "d41d8cd98f00b204e9800998ecf8427e";
        return true;
    }
    return false;
}

bool downloadFirmware(const String& url) {
    if (url.length() > 0) {
        ota_download_success = true;
        return true;
    }
    return false;
}

bool verifyFirmware(const String& md5) {
    if (md5.length() > 0) {
        ota_verify_success = true;
        return true;
    }
    return false;
}

bool applyFirmwareUpdate() {
    ota_apply_success = true;
    return true;
}

void resetOtaState() {
    ota_update_available = false;
    ota_firmware_url.clear();
    ota_firmware_version.clear();
    ota_firmware_md5.clear();
    ota_download_success = false;
    ota_verify_success = false;
    ota_apply_success = false;
} 