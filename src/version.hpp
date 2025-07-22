#pragma once

#include "config.hpp"

namespace version {

/**
 * @brief Get the firmware version as an integer
 * @return Firmware version as integer (e.g., 100 for v1.0.0, 110 for v1.1.0)
 */
inline int getFirmwareVersionInt() {
    return config::FirmwareVersionInt;
}

/**
 * @brief Get the firmware version string
 * @return Pointer to the firmware version string
 */
const char* getFirmwareVersion();

/**
 * @brief Print firmware version information to Serial
 */
void printFirmwareVersion();

/**
 * @brief Check if this is a development build
 * @return true if development build, false if release build
 */
inline bool isDevelopmentBuild() {
    return config::FirmwareVersionInt == 0;
}

/**
 * @brief Get build information string
 * @return Build information including version and build type
 */
const char* getBuildInfo();

}        // namespace version