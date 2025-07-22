#pragma once

// Centralised configuration constants for the project.
//
// Moving all compile–time configuration into this header makes it easier to
// maintain different hardware revisions or build variants.  Changing the
// value in a single place is enough and prevents the usual problems that come
// with duplicated literal numbers or pre-processor macros spread across the
// code base.
//
// Only constexpr variables (and not macros) are exposed here.  This gives the
// compiler type information, avoids possible name collisions and honours the
// usual C++ scoping rules.  All constants live in the `config` namespace so
// that they do not pollute the global namespace.

namespace config {

// Debug control --------------------------------------------------------------
// Debug is now compile-time disabled for optimized builds
constexpr bool EnableDebug = false;

// SX127x / LMIC wiring -------------------------------------------------------
constexpr int PinLmicNss  = 18;
constexpr int PinLmicRst  = 14;
constexpr int PinLmicDio0 = 26;
constexpr int PinLmicDio1 = 33;
constexpr int PinLmicDio2 = 32;

// LoRaWAN / payload ----------------------------------------------------------
constexpr int MaxPayloadSize = 200;        // Increased to accommodate OTA messages

// Sensors --------------------------------------------------------------------
constexpr int SoilSensorPin = 34;          // ADC1_CH6
constexpr int AirValue      = 2200;        // Dry calibration value
constexpr int WaterValue    = 380;         // Wet calibration value

// Misc -----------------------------------------------------------------------
constexpr int MaxSensorRead = 1;

// Firmware Version -----------------------------------------------------------
// The FIRMWARE_VERSION is set by the CI/CD pipeline during compilation
// Format: 3-digit integer (e.g., 100 for v1.0.0, 110 for v1.1.0, 112 for v1.1.2)
#ifdef FIRMWARE_VERSION
constexpr int FirmwareVersionInt = static_cast<int>(FIRMWARE_VERSION);
#else
constexpr int FirmwareVersionInt = 0;        // Development build
#endif

}        // namespace config
