#ifndef ARDUINO_H
#define ARDUINO_H

#include <cstdint>
#include <cstring>
#include <string>
#include <algorithm>

// Arduino String class
class String {
private:
    std::string str_;

public:
    String() : str_("") {}
    String(const char* s) : str_(s ? s : "") {}
    String(const std::string& s) : str_(s) {}
    
    const char* c_str() const { return str_.c_str(); }
    size_t length() const { return str_.length(); }
    bool isEmpty() const { return str_.empty(); }
    bool equals(const String& other) const { return str_ == other.str_; }
    bool equalsIgnoreCase(const String& other) const { 
        // Simple case-insensitive comparison
        std::string lower1 = str_, lower2 = other.str_;
        std::transform(lower1.begin(), lower1.end(), lower1.begin(), ::tolower);
        std::transform(lower2.begin(), lower2.end(), lower2.begin(), ::tolower);
        return lower1 == lower2;
    }
    
    String substring(size_t from, size_t to) const {
        if (from >= str_.length() || to <= from) {
            return String();
        }
        return String(str_.substr(from, to - from));
    }
    
    String& operator=(const char* s) { str_ = s ? s : ""; return *this; }
    String& operator=(const String& s) { str_ = s.str_; return *this; }
    
    operator const char*() const { return str_.c_str(); }
};

// Arduino Serial
class SerialClass {
public:
    void begin(int) {}
    void print(const char*) {}
    void print(int) {}
    void print(int, int) {}  // value, base
    void print(unsigned int) {}
    void print(unsigned int, int) {}  // value, base
    void print(long) {}
    void print(long, int) {}  // value, base
    void print(unsigned long) {}
    void print(unsigned long, int) {}  // value, base
    void print(float) {}
    void print(const String&) {}
    void println(const char*) {}
    void println(int) {}
    void println(int, int) {}  // value, base
    void println(unsigned int) {}
    void println(unsigned int, int) {}  // value, base
    void println(long) {}
    void println(long, int) {}  // value, base
    void println(unsigned long) {}
    void println(unsigned long, int) {}  // value, base
    void println(float) {}
    void println(const String&) {}
    void println() {}
    void printf(const char*, ...) {}
    void flush() {}
};

extern SerialClass Serial;

// Arduino delay functions
void delay(unsigned long);
unsigned long millis();

// Arduino pin functions
void pinMode(int, int);
void digitalWrite(int, int);
int digitalRead(int);
int analogRead(int);

// Arduino random functions
void randomSeed(unsigned long);
int random(int);
int random(int, int);

// Arduino constants
#define HIGH 1
#define LOW 0
#define DEC 10
#define HEX 16
#define F(x) x

// Arduino map function
template <typename T>
static inline T map(T x, T in_min, T in_max, T out_min, T out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Float overload for mixed type calls
static inline float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// memcpy_P stub
#define memcpy_P(dest, src, n) std::memcpy((dest), (src), (n))

// Settings functions
void settings_clear();
void loadSetings();
void initMenu();
void saveConfigCallback();

// Global instances for WiFi and FastLED
extern class WiFiClass WiFi;
extern class FastLEDClass FastLED;

// ESP32 specific
class ESPClass {
public:
    void restart() {}
};

extern ESPClass ESP;

// WiFi global variables for UNIT_TEST
#ifdef UNIT_TEST
extern char wifi_ssid_str[32];
extern char wifi_password_str[64];
#endif

#endif // ARDUINO_H
