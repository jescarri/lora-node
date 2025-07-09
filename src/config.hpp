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

// SX127x / LMIC wiring -------------------------------------------------------
constexpr int PinLmicNss  = 18;
constexpr int PinLmicRst  = 14;
constexpr int PinLmicDio0 = 26;
constexpr int PinLmicDio1 = 33;
constexpr int PinLmicDio2 = 32;

// LoRaWAN / payload ----------------------------------------------------------
constexpr int MaxPayloadSize = 51;

// Sensors --------------------------------------------------------------------
constexpr int SoilSensorPin = 34;     // ADC1_CH6
constexpr int AirValue      = 2200;   // Dry calibration value
constexpr int WaterValue    = 380;    // Wet calibration value

// Misc -----------------------------------------------------------------------
constexpr int MaxSensorRead = 1;

} // namespace config

