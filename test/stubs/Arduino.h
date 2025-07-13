#ifndef ARDUINO_H
#define ARDUINO_H

#include <string>
#include <cstdint>
#include <cstring>
#include <cctype>

// Minimal Arduino String class
class String {
private:
    std::string str;

public:
    String() : str("") {}
    String(const char* s) : str(s ? s : "") {}
    String(const char* s, size_t n) : str(s ? std::string(s, n) : "") {}
    String(const String& s) : str(s.str) {}
    String(const std::string& s) : str(s) {}
    
    const char* c_str() const { return str.c_str(); }
    int length() const { return static_cast<int>(str.length()); }
    bool isEmpty() const { return str.empty(); }
    
    String& operator=(const char* s) { str = s ? s : ""; return *this; }
    String& operator=(const String& s) { str = s.str; return *this; }
    
    bool operator==(const String& s) const { return str == s.str; }
    bool operator==(const char* s) const { return str == (s ? s : ""); }
    
    char operator[](int index) const { return str[index]; }
    char& operator[](int index) { return str[index]; }
    
    String substring(int start, int end = -1) const {
        if (end == -1) end = static_cast<int>(str.length());
        if (start < 0) start = 0;
        if (end > static_cast<int>(str.length())) end = static_cast<int>(str.length());
        if (start >= end) return String("");
        return String(str.substr(start, end - start));
    }
    
    int toInt() const {
        try { return std::stoi(str); } catch (...) { return 0; }
    }
    
    float toFloat() const {
        try { return std::stof(str); } catch (...) { return 0.0f; }
    }
    
    // ArduinoJson compatibility
    int read() { 
        static size_t read_pos = 0;
        if (read_pos < str.length()) return str[read_pos++];
        return -1;
    }
};

// Basic Arduino types
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;

// Basic Arduino constants
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define DEC 10
#define HEX 16
#define F(x) x

// Basic Arduino functions
void delay(unsigned long ms);
unsigned long millis();
void digitalWrite(int pin, int value);
int digitalRead(int pin);
void pinMode(int pin, int mode);
int analogRead(int pin);
int map(int value, int fromLow, int fromHigh, int toLow, int toHigh);
float map(float value, float fromLow, float fromHigh, float toLow, float toHigh);

// memcpy_P stub
#define memcpy_P(dest, src, n) std::memcpy((dest), (src), (n))

// Minimal Serial class
class SerialClass {
public:
    void begin(unsigned long baud) {}
    void print(const char* str) {}
    void print(const String& str) {}
    void print(int value) {}
    void print(int value, int base) {}
    void print(unsigned int value) {}
    void print(unsigned int value, int base) {}
    void print(long value) {}
    void print(long value, int base) {}
    void print(unsigned long value) {}
    void print(unsigned long value, int base) {}
    void print(float value) {}
    void println(const char* str) {}
    void println(const String& str) {}
    void println(int value) {}
    void println(int value, int base) {}
    void println(unsigned int value) {}
    void println(unsigned int value, int base) {}
    void println(long value) {}
    void println(long value, int base) {}
    void println(unsigned long value) {}
    void println(unsigned long value, int base) {}
    void println(float value) {}
    void println() {}
    void printf(const char* format, ...) {}
    int available() { return 0; }
    int read() { return -1; }
};

extern SerialClass Serial;

// ESP32 specific
class ESPClass {
public:
    void restart() {}
};

extern ESPClass ESP;

#endif // ARDUINO_H
