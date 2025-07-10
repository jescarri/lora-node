// Minimal stub of the WiFiManager library just enough for unit-testing on the
// native host.  The real library is only required on the ESP32 target – in
// the host environment we merely need the types and methods referenced by the
// production code so that it links successfully.

#pragma once

#include <cstddef>

class WiFiManagerParameter {
public:
    WiFiManagerParameter(const char * /*id*/, const char * /*placeholder*/,
                         char *value, int /*length*/)
        : value_{value} {}

    const char *getValue() const { return value_; }

private:
    char *value_;
};

class WiFiManager {
public:
    void setMinimumSignalQuality(int /*quality*/) {}
    void setRemoveDuplicateAPs(bool /*remove*/) {}

    using SaveCallback = void (*)();
    void setSaveParamsCallback(SaveCallback /*cb*/) {}

    void addParameter(WiFiManagerParameter * /*param*/) {}

    void setMenu(const char ** /*menu*/, std::size_t /*cnt*/) {}

    void setConfigPortalTimeout(int /*seconds*/) {}

    bool startConfigPortal(const char * /*ssid*/) { return true; }
};

