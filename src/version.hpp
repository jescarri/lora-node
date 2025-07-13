#pragma once

#include "config.hpp"

namespace version {

/**
 * @brief Get the firmware version as a float
 * @return Firmware version as float (e.g., 1.0, 1.1, 2.0)
 */
inline float getFirmwareVersionFloat() {
    return config::FirmwareVersionFloat;
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
    return config::FirmwareVersionFloat == 0.0f;
}

/**
 * @brief Get build information string
 * @return Build information including version and build type
 */
const char* getBuildInfo();

} // namespace version 