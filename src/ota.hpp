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

// Function declarations
void handleDownlinkMessage(uint8_t* data, uint8_t dataLen);
bool parseOtaMessage(const uint8_t* data, uint8_t dataLen, OtaUpdateInfo& updateInfo);
bool downloadAndInstallFirmware(const OtaUpdateInfo& updateInfo);
bool verifyMd5Sum(const uint8_t* data, size_t dataLen, const String& expectedMd5);
void reportFirmwareVersion(CayenneLPP& lpp);
bool testWifiConnection(const String& ssid, const String& password);

// Firmware version reporting - will be resolved at runtime
int getFirmwareVersionInt();

#endif        // OTA_HPP_
