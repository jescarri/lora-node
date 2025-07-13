#ifndef ARDUINOJSON_H
#define ARDUINOJSON_H

#include <Arduino.h>
#include <string>
#include <cstring>

class DynamicJsonDocument {
private:
    size_t capacity_;
    void* data_;
    std::string url_;
    std::string md5sum_;
    std::string version_;

public:
    DynamicJsonDocument(size_t capacity) : capacity_(capacity), data_(nullptr) {
        // Set default test values
        url_ = "https://example.com/firmware.bin";
        md5sum_ = "d41d8cd98f00b204e9800998ecf8427e";
        version_ = "101";
    }
    ~DynamicJsonDocument() {}
    
    bool containsKey(const char* key) const { 
        return (strcmp(key, "url") == 0 || 
                strcmp(key, "md5sum") == 0 || 
                strcmp(key, "version") == 0);
    }
    
    String operator[](const char* key) const { 
        if (strcmp(key, "url") == 0) return String(url_.c_str());
        if (strcmp(key, "md5sum") == 0) return String(md5sum_.c_str());
        if (strcmp(key, "version") == 0) return String(version_.c_str());
        return String("");
    }
};

enum DeserializationErrorType {
    DeserializationError_Ok,
    DeserializationError_InvalidInput,
    DeserializationError_NoMemory,
    DeserializationError_EmptyInput,
    DeserializationError_IncompleteInput
};

class DeserializationError {
private:
    DeserializationErrorType error_;

public:
    DeserializationError(DeserializationErrorType error) : error_(error) {}
    bool operator!() const { return error_ == DeserializationError_Ok; }
    const char* c_str() const { return "Test error"; }
};

DeserializationError deserializeJson(DynamicJsonDocument& doc, const String& input) {
    // Convert Arduino String to std::string for native compatibility
    std::string inputStr = input.c_str();
    
    // Check for malformed JSON
    if (inputStr.find("invalid json") != std::string::npos) {
        return DeserializationError(DeserializationError_InvalidInput);
    }
    
    // Check for missing fields
    if (inputStr.find("\"md5sum\"") == std::string::npos || 
        inputStr.find("\"version\"") == std::string::npos) {
        return DeserializationError(DeserializationError_InvalidInput);
    }
    
    return DeserializationError(DeserializationError_Ok);
}

#endif // ARDUINOJSON_H 