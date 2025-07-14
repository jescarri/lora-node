#include "version.hpp"
#include <Arduino.h>
#include <cstdlib>

namespace version {

const char* getFirmwareVersion() {
    static std::string versionString;

    if (isDevelopmentBuild()) {
        return "dev";
    } else {
        // Convert integer to semantic version string (e.g., 110 -> "v1.1.0")
        int versionInt = getFirmwareVersionInt();
        int major      = versionInt / 100;
        int minor      = (versionInt % 100) / 10;
        int patch      = versionInt % 10;

        versionString = "v" + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
        return versionString.c_str();
    }
}

void printFirmwareVersion() {
    Serial.println("=== Firmware Information ===");
    Serial.print("Version: ");
    Serial.println(getFirmwareVersion());

    if (isDevelopmentBuild()) {
        Serial.println("Build Type: Development");
    } else {
        Serial.println("Build Type: Release");
    }

    Serial.print("Build Info: ");
    Serial.println(getBuildInfo());
    Serial.println("===========================");
}

const char* getBuildInfo() {
    static std::string buildInfo;

    if (isDevelopmentBuild()) {
        buildInfo = "Development Build";
    } else {
        buildInfo = "Release: " + std::string(getFirmwareVersion());
    }

    return buildInfo.c_str();
}

}        // namespace version
