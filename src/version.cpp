#include "version.hpp"
#include <Arduino.h>
#include <cstdlib>

namespace version {

const char* getFirmwareVersion() {
    static char versionString[16];
    
    if (isDevelopmentBuild()) {
        return "dev";
    } else {
        // Convert float to string (e.g., 1.0 -> "1.0")
        snprintf(versionString, sizeof(versionString), "%.1f", getFirmwareVersionFloat());
        return versionString;
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
    static char buildInfo[64];
    
    if (isDevelopmentBuild()) {
        snprintf(buildInfo, sizeof(buildInfo), "Development Build");
    } else {
        snprintf(buildInfo, sizeof(buildInfo), "Release v%s", getFirmwareVersion());
    }
    
    return buildInfo;
}

} // namespace version 