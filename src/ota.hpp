#ifndef OTA_HPP_
#define OTA_HPP_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.hpp"
#ifndef UNIT_TEST
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#endif
#include <CayenneLPP.h>

// OTA Update structure
struct OtaUpdateInfo {
    String url;
    String md5sum;
    String version;
    bool valid;
};

// OTA state management
extern volatile bool ota_in_progress;

// Function declarations
void handleDownlinkMessage(uint8_t* data, uint8_t dataLen);
bool parseOtaMessage(const uint8_t* data, uint8_t dataLen, OtaUpdateInfo& updateInfo);
bool downloadAndInstallFirmware(const OtaUpdateInfo& updateInfo);
bool verifyMd5Sum(const uint8_t* data, size_t dataLen, const String& expectedMd5);
void reportFirmwareVersion(CayenneLPP& lpp);
bool testWifiConnection(const String& ssid, const String& password);

// OTA state management functions
void setOtaInProgress(bool inProgress);
bool isOtaInProgress();

// Firmware version reporting - will be resolved at runtime
int getFirmwareVersionInt();

#endif        // OTA_HPP_
