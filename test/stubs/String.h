#pragma once
#include <string>
class String {
    std::string data;
public:
    String() : data("") {}
    String(const char* s) : data(s ? s : "") {}
    String(const std::string& s) : data(s) {}
    String(const String& s) : data(s.data) {}
    String& operator=(const String& s) { data = s.data; return *this; }
    const char* c_str() const { return data.c_str(); }
    int length() const { return static_cast<int>(data.length()); }
    bool operator==(const String& other) const { return data == other.data; }
}; 