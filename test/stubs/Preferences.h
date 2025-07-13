#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <Arduino.h>
#include <string>
#include <unordered_map>
#include <vector>

class Preferences {
private:
    std::unordered_map<std::string, std::string> stringStore;
    std::unordered_map<std::string, long> longStore;
    std::unordered_map<std::string, std::vector<uint8_t>> bytesStore;

public:
    bool begin(const char* name, bool readOnly = false) { return true; }
    void end() {}
    
    String getString(const char* key, const String& defaultValue = "") { 
        auto it = stringStore.find(key);
        return (it != stringStore.end()) ? String(it->second.c_str()) : defaultValue;
    }
    void putString(const char* key, const String& value) {
        stringStore[key] = value.c_str();
    }
    
    bool getBool(const char* key, bool defaultValue = false) { return defaultValue; }
    void putBool(const char* key, bool value) {}
    
    int getInt(const char* key, int defaultValue = 0) { return defaultValue; }
    void putInt(const char* key, int value) {}
    
    long getLong(const char* key, long defaultValue = 0) {
        auto it = longStore.find(key);
        return (it != longStore.end()) ? it->second : defaultValue;
    }
    void putLong(const char* key, long value) {
        longStore[key] = value;
    }
    
    bool isKey(const char* key) {
        return stringStore.find(key) != stringStore.end() ||
               longStore.find(key) != longStore.end() ||
               bytesStore.find(key) != bytesStore.end();
    }
    
    size_t putBytes(const char* key, const void* value, size_t len) {
        const uint8_t* data = static_cast<const uint8_t*>(value);
        bytesStore[key] = std::vector<uint8_t>(data, data + len);
        return len;
    }
    
    size_t getBytesLength(const char* key) {
        auto it = bytesStore.find(key);
        return (it != bytesStore.end()) ? it->second.size() : 0;
    }
    
    size_t getBytes(const char* key, void* buf, size_t len) {
        auto it = bytesStore.find(key);
        if (it == bytesStore.end()) return 0;
        
        size_t copyLen = std::min(len, it->second.size());
        std::copy(it->second.begin(), it->second.begin() + copyLen, 
                  static_cast<uint8_t*>(buf));
        return copyLen;
    }
    
    void remove(const char* key) {
        stringStore.erase(key);
        longStore.erase(key);
        bytesStore.erase(key);
    }
    
    void clear() {
        stringStore.clear();
        longStore.clear();
        bytesStore.clear();
    }
    
    bool hasKey(const char* key) { return isKey(key); }
};

#endif // PREFERENCES_H
