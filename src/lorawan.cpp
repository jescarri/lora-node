#define LORAWAN_CPP_IMPLEMENTATION
#include "lorawan.hpp"
#include "lorawan_settings.hpp"
#include "utils.hpp"
#include "ota.hpp"
#include <cstring>
#include <cctype>
#include "lorawan_settings.hpp"
#include <Adafruit_MAX1704X.h>
#include <array>

// PROGMEM string constants for LoRaWAN
const char PROGMEM msg_lmic_opmode[]            = "LMIC.opmode: ";
const char PROGMEM msg_lmic_seqno_up[]          = "LMIC.seqnoUp = ";
const char PROGMEM msg_lmic_global_duty_rate[]  = "LMIC.globalDutyRate = ";
const char PROGMEM msg_lmic_global_duty_avail[] = "LMIC.globalDutyAvail = ";
const char PROGMEM msg_lmic_band_plan_next_tx[] = "LMICbandplan_nextTx = ";
const char PROGMEM msg_os_get_time[]            = "os_getTime = ";
const char PROGMEM msg_lmic_txend[]             = "LMIC.txend = ";
const char PROGMEM msg_lmic_txchnl[]            = "LMIC.txChnl = ";
const char PROGMEM msg_lmic_version[]           = "LMIC: ";
const char PROGMEM msg_oxticks[]                = " osTicks, ";
const char PROGMEM msg_sec[]                    = " sec";
const char PROGMEM msg_separator[]              = "-----";
const char PROGMEM msg_do_send[]                = "do_send";
const char PROGMEM msg_lmic_opmode_equals[]     = "LMIC.opmode= ";
const char PROGMEM msg_app_eui[]                = "app_eui: ";
const char PROGMEM msg_dev_eui[]                = "dev_eui: ";
const char PROGMEM msg_app_key[]                = "app_key: ";
const char PROGMEM msg_charge_rate[]            = "----ChargeRate: ";
const char PROGMEM msg_x_format[]               = "X: %f";
const char PROGMEM msg_moisture_format[]        = "Moisture ADC: %f, Moisture Percentage: %f, vBat %f\n\n";
const char PROGMEM msg_error_app_eui[]          = "ERROR: app_eui string missing or too short";
const char PROGMEM msg_error_app_eui_hex[]      = "ERROR: app_eui contains non-hex digits";
const char PROGMEM msg_error_dev_eui[]          = "ERROR: dev_eui string missing or too short";
const char PROGMEM msg_error_dev_eui_hex[]      = "ERROR: dev_eui contains non-hex digits";
const char PROGMEM msg_error_app_key[]          = "ERROR: app_key string missing or too short";
const char PROGMEM msg_error_app_key_hex[]      = "ERROR: app_key contains non-hex digits";

sensorData sd;

void LoraWANPrintLMICOpmode(void) {
    Serial.print(F("LMIC.opmode: "));
    if (LMIC.opmode & OP_NONE) {
        Serial.print(F("OP_NONE "));
    }
    if (LMIC.opmode & OP_SCAN) {
        Serial.print(F("OP_SCAN "));
    }
    if (LMIC.opmode & OP_TRACK) {
        Serial.print(F("OP_TRACK "));
    }
    if (LMIC.opmode & OP_JOINING) {
        Serial.print(F("OP_JOINING "));
    }
    if (LMIC.opmode & OP_TXDATA) {
        Serial.print(F("OP_TXDATA "));
    }
    if (LMIC.opmode & OP_POLL) {
        Serial.print(F("OP_POLL "));
    }
    if (LMIC.opmode & OP_REJOIN) {
        Serial.print(F("OP_REJOIN "));
    }
    if (LMIC.opmode & OP_SHUTDOWN) {
        Serial.print(F("OP_SHUTDOWN "));
    }
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.print(F("OP_TXRXPEND "));
    }
    if (LMIC.opmode & OP_RNDTX) {
        Serial.print(F("OP_RNDTX "));
    }
    if (LMIC.opmode & OP_PINGINI) {
        Serial.print(F("OP_PINGINI "));
    }
    if (LMIC.opmode & OP_PINGABLE) {
        Serial.print(F("OP_PINGABLE "));
    }
    if (LMIC.opmode & OP_NEXTCHNL) {
        Serial.print(F("OP_NEXTCHNL "));
    }
    if (LMIC.opmode & OP_LINKDEAD) {
        Serial.print(F("OP_LINKDEAD "));
    }
    if (LMIC.opmode & OP_TESTMODE) {
        Serial.print(F("OP_TESTMODE "));
    }
    if (LMIC.opmode & OP_UNJOIN) {
        Serial.print(F("OP_UNJOIN "));
    }
}

void LoraWANDebug(const lmic_t& lmic_check) {
    // Debug output now compile-time disabled for optimized builds
}

void PrintLMICVersion() {
    Serial.print(F("LMIC: "));
    Serial.print(ARDUINO_LMIC_VERSION_GET_MAJOR(ARDUINO_LMIC_VERSION));
    Serial.print(F("."));
    Serial.print(ARDUINO_LMIC_VERSION_GET_MINOR(ARDUINO_LMIC_VERSION));
    Serial.print(F("."));
    Serial.print(ARDUINO_LMIC_VERSION_GET_PATCH(ARDUINO_LMIC_VERSION));
    Serial.print(F("."));
    Serial.println(ARDUINO_LMIC_VERSION_GET_LOCAL(ARDUINO_LMIC_VERSION));
}

void onEvent(ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            leds[0] = CRGB::DarkOrange;
            FastLED.show();
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            {
                u4_t netid        = 0;
                devaddr_t devaddr = 0;
                std::array<u1_t, 16> nwkKey{};
                std::array<u1_t, 16> artKey{};
                LMIC_getSessionKeys(&netid, &devaddr, nwkKey.data(), artKey.data());
                Serial.print("netid: ");
                Serial.println(netid, DEC);
                Serial.print("devaddr: ");
                Serial.println(devaddr, HEX);
                Serial.print("artKey: ");
                for (auto val : artKey) {
                    Serial.print(val, HEX);
                }
                Serial.println("");
                Serial.print("nwkKey: ");
                for (auto val : nwkKey) {
                    Serial.print(val, HEX);
                }
                Serial.println("");
            }
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
            // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            leds[0] = CRGB::Green;
            FastLED.show();
            delay(1000);
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            // Transmit completed, includes waiting for RX windows.
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            // Check if downlink was received
            if (LMIC.dataLen != 0 || LMIC.dataBeg != 0) {
                // Downlink data received, process OTA update if present
                uint8_t fPort = 0;
                if (LMIC.txrxFlags & TXRX_PORT) {
                    fPort = LMIC.frame[LMIC.dataBeg - 1];
                }

                // Handle OTA update messages on port 1
                if (fPort >= 1 && fPort <= OTA_MAX_CHUNKS) {
                    uint8_t* downlinkData = &LMIC.frame[LMIC.dataBeg];
                    uint8_t downlinkLen   = LMIC.dataLen;
                    handleDownlinkMessage(downlinkData, downlinkLen, fPort);
                }
            }
            enableSleep_ = true;
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        /*
            || This event is defined but not used in the code. No
            || point in wasting codespace on it.
            ||
            || case EV_SCAN_FOUND:
            ||    Serial.println(F("EV_SCAN_FOUND"));
            ||    break;
        */
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;
        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned)ev);
            break;
    }
}

void do_send(osjob_t* /* j */) {
    lpp.reset();
    ReadSensors();
    lpp.addGenericSensor(0, static_cast<float>(sd.soilMoistureValue));
    lpp.addVoltage(1, sd.vBat);
    lpp.addPercentage(2, sd.soilMoisturePercentage);
    lpp.addSwitch(3, sd.lipoGaugeOk);
    lpp.addPercentage(4, sd.batPercent);
    lpp.addPercentage(5, sd.batRate);
    lpp.addGenericSensor(6, static_cast<float>(get_calibration_air_value()));
    lpp.addGenericSensor(7, static_cast<float>(get_calibration_water_value()));
    lpp.addGenericSensor(8, static_cast<float>(get_sleep_time_seconds()));
    lpp.addGenericSensor(9, static_cast<float>(get_previous_runtime()));
    reportFirmwareVersion(lpp);

    // Check if there is not a current TX/RX job running
    Serial.println(FPSTR(msg_do_send));
    Serial.print(FPSTR(msg_lmic_opmode_equals));
    Serial.println(LMIC.opmode);
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(), 0);
        Serial.println(F("Packet queued"));
    }
}

// ToDo: Refactor hex string to u1_t array conversion
void os_getArtEui(u1_t* buf) {
    const String cfg = settings_get_string("app_eui");

    if (cfg.isEmpty() || cfg.length() < 16) {
        Serial.println(FPSTR(msg_error_app_eui));
        memset(buf, 0, 8);
        return;
    }

    std::array<u1_t, 8> app_eui{};

    for (int c = 0, i = 0; c < 8; ++c, i += 2) {
        if (!isxdigit(cfg[i]) || !isxdigit(cfg[i + 1])) {
            Serial.println(FPSTR(msg_error_app_eui_hex));
            memset(buf, 0, 8);
            return;
        }

        std::string t = cfg.substring(i, i + 2).c_str();
        app_eui[c]    = static_cast<u1_t>(strtoul(t.c_str(), nullptr, 16));
    }

    Serial.print(FPSTR(msg_app_eui));
    for (auto val : app_eui) {
        Serial.print(val, HEX);
    }
    Serial.println();

    memcpy_P(buf, app_eui.data(), 8);
}

void os_getDevEui(u1_t* buf) {
    const String cfg = settings_get_string("dev_eui");

    if (cfg.isEmpty() || cfg.length() < 16) {
        Serial.println("ERROR: dev_eui string missing or too short");
        memset(buf, 0, 8);
        return;
    }

    std::array<u1_t, 8> dev_eui{};

    for (int c = 0, i = 0; c < 8; ++c, i += 2) {
        if (!isxdigit(cfg[i]) || !isxdigit(cfg[i + 1])) {
            Serial.println("ERROR: dev_eui contains non-hex digits");
            memset(buf, 0, 8);
            return;
        }

        std::string t = cfg.substring(i, i + 2).c_str();
        dev_eui[c]    = static_cast<u1_t>(strtoul(t.c_str(), nullptr, 16));
    }

    Serial.print("dev_eui: ");
    for (u1_t b : dev_eui) {
        Serial.print(b, HEX);
    }
    Serial.println();

    memcpy_P(buf, dev_eui.data(), 8);
}

void os_getDevKey(u1_t* buf) {
    const String cfg = settings_get_string("app_key");

    // Validate the expected length (32 hex chars → 16 bytes)
    if (cfg.isEmpty() || cfg.length() < 32) {
        Serial.println("ERROR: app_key string missing or too short");
        memset(buf, 0, 16);
        return;
    }

    std::array<u1_t, 16> app_key{};

    for (int c = 0, i = 0; c < 16; ++c, i += 2) {
        // Extract two hex digits as a String
        std::string twoChars = cfg.substring(i, i + 2).c_str();

        if (!isxdigit(twoChars[0]) || !isxdigit(twoChars[1])) {
            Serial.println("ERROR: app_key contains non-hex digits");
            memset(buf, 0, 16);
            return;
        }

        app_key[c] = static_cast<u1_t>(strtoul(twoChars.c_str(), nullptr, 16));
    }

    Serial.print("app_key: ");
    for (u1_t b : app_key) {
        Serial.print(b, HEX);
    }
    Serial.println();

    memcpy_P(buf, app_key.data(), 16);
}

void ReadSensors() {
    sd.soilMoisturePercentage = 0.0f;
    sd.soilMoistureValue      = 0.0f;
    sd.lipoGaugeOk            = maxLipoFound;
    if (maxLipoFound == true) {
        sd.vBat       = maxlipo.cellVoltage();
        sd.batPercent = maxlipo.cellPercent();
        sd.batRate    = maxlipo.chargeRate();
    }
    for (int i = 0; i < MAX_SENSOR_READ; i++) {
        float a = static_cast<float>(analogRead(config::SoilSensorPin));
        sd.soilMoistureValue += a;
        delay(10);
    }
    float t                   = sd.soilMoistureValue / static_cast<float>(MAX_SENSOR_READ);
    sd.soilMoistureValue      = t;
    float x                   = static_cast<float>(map(static_cast<long>(sd.soilMoistureValue),
                                                       get_calibration_air_value(),
                                                       get_calibration_water_value(), 0, 100));
    sd.soilMoisturePercentage = abs(x);
}
