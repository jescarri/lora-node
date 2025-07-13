#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <Arduino.h>
#include <WiFi.h>

#define HTTP_CODE_OK 200
#define UPDATE_SIZE_UNKNOWN 0

class HTTPClient {
public:
    HTTPClient() {}
    ~HTTPClient() {}
    
    void begin(const String& url) {}
    int GET() { return HTTP_CODE_OK; }
    int getSize() { return 1024; }
    String errorToString(int code) { return "Test error"; }
    void end() {}
    WiFiClient* getStreamPtr() { return nullptr; }
};

class Update {
public:
    static bool begin(size_t size) { return true; }
    static size_t writeStream(WiFiClient& client) { return 1024; }
    static bool setMD5(const char* md5) { return true; }
    static bool end() { return true; }
};

#endif // HTTPCLIENT_H 