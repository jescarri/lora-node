#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <variant>

class Preferences {
public:
    Preferences() = default;

    void begin(const char * /*ns*/, bool /*ro*/) {
        // For the purposes of unit tests we reset all stored values on every
        // begin() call to emulate a fresh namespace.
        store_.clear();
    }

    bool isKey(const char *key) {
        return store_.count(key) != 0;
    }

    size_t putBytes(const char *key, const void *value, size_t len) {
        const uint8_t *ptr = static_cast<const uint8_t *>(value);
        store_[key] = std::vector<uint8_t>(ptr, ptr + len);
        return len;
    }

    size_t getBytesLength(const char *key) {
        if (!isKey(key)) return 0;
        return std::get<std::vector<uint8_t>>(store_[key]).size();
    }

    size_t getBytes(const char *key, void *buf, size_t len) {
        if (!isKey(key)) return 0;
        auto &vec = std::get<std::vector<uint8_t>>(store_[key]);
        size_t n = std::min(len, vec.size());
        std::copy(vec.begin(), vec.begin() + n, static_cast<uint8_t *>(buf));
        return n;
    }

    void remove(const char *key) {
        store_.erase(key);
    }

    void putLong(const char *key, long value) {
        store_[key] = static_cast<long long>(value);
    }

    long getLong(const char *key, long def = 0) {
        if (!isKey(key)) return def;
        return static_cast<long>(std::get<long long>(store_[key]));
    }

    void putString(const char *key, const char *value) {
        store_[key] = std::string(value);
    }

    String getString(const char *key) {
        if (!isKey(key)) return String();
        return String(std::get<std::string>(store_[key]));
    }

    void putBool(const char *key, bool value) {
        store_[key] = value;
    }

    bool getBool(const char *key, bool def = false) {
        if (!isKey(key)) return def;
        return std::get<bool>(store_[key]);
    }

private:
    using Variant = std::variant<std::vector<uint8_t>, long long, std::string, bool>;
    std::unordered_map<std::string, Variant> store_;
};
