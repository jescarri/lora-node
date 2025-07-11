#ifndef LORAWAN_HPP_
#define LORAWAN_HPP_

#include <Arduino.h>
#include <lmic.h>
#include <Adafruit_MAX1704X.h>
#include <CayenneLPP.h>
#include <FastLED.h>
#include <hal/hal.h>

#include "config.hpp"

// Re-export selected configuration constants so that existing code that still
// uses the former identifiers keeps compiling.  New code should prefer the
// names in the `config` namespace directly.

constexpr int MAX_PAYLOAD_SIZE = config::MaxPayloadSize;
constexpr int MAX_SENSOR_READ  = config::MaxSensorRead;

#define SOIL_SENSOR_PIN config::SoilSensorPin

// Keep compatibility symbols for legacy code.  These are intentionally *not*
// placed inside a namespace so that they behave 1:1 like the previous
// pre-processor macros.
#define PIN_LMIC_NSS  config::PinLmicNss
#define PIN_LMIC_RST  config::PinLmicRst
#define PIN_LMIC_DIO0 config::PinLmicDio0
#define PIN_LMIC_DIO1 config::PinLmicDio1
#define PIN_LMIC_DIO2 config::PinLmicDio2

struct sensorData {
    float soilMoisturePercentage;
    float soilMoistureValue;
    bool lipoGaugeOk;
    float vBat;
    float batPercent;
    float batRate;
};

extern const lmic_pinmap lmic_pins;
extern volatile bool enableSleep_;
extern osjob_t sendjob;
extern CRGB leds[1];

extern CayenneLPP lpp;
extern bool maxLipoFound;
extern Adafruit_MAX17048 maxlipo;

void LoraWANPrintLMICOpmode(void);
void LoraWANDebug(const lmic_t& lmic_check);
void PrintLMICVersion();
void onEvent(ev_t ev);
void do_send(osjob_t *j);
void ReadSensors();

void os_getArtEui(u1_t *buf);
void os_getDevEui(u1_t *buf);
void os_getDevKey(u1_t *buf);

#endif
