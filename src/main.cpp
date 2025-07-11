#ifndef UNIT_TEST
#include "WiFi.h"
#include "driver/adc.h"


#include <lmic.h>

#include "esp_bt.h"

#include "esp_sleep.h"
#include "lorawan.hpp"
#include "lorawan_settings.hpp"
#include "menu.hpp"
#include <array>
#include <Adafruit_MAX1704X.h>
#include <FastLED.h>
#include <Wire.h>
#include <hal/hal.h>

constexpr int VCC_ENA_PIN = 13;
constexpr int START_WEB_CONFIG_PIN = 16;
constexpr int NUM_LEDS = 1;
constexpr int LED_DATA_PIN = 17;
constexpr unsigned long long uS_TO_S_FACTOR = 1000000ULL;

lmic_t SETTINGS_LMIC;
CayenneLPP lpp(MAX_PAYLOAD_SIZE);
Adafruit_MAX17048 maxlipo;
CRGB leds[NUM_LEDS];
bool maxLipoFound = false;

// Active in LOW, normal operation is HIGH
bool startWebConfig = false;

void PrintRuntime();
void GoDeepSleep();
void ReadSensors();

volatile bool enableSleep_ = true;
unsigned long entry;

osjob_t sendjob;

const lmic_pinmap lmic_pins = {
    .nss            = PIN_LMIC_NSS,
    .rxtx           = LMIC_UNUSED_PIN,
    .rst            = PIN_LMIC_RST,
    .dio            = {PIN_LMIC_DIO0, PIN_LMIC_DIO1, PIN_LMIC_DIO2},
    .rxtx_rx_active = 0,
    .spi_freq       = 8000000,
};

static constexpr std::array<gpio_num_t, 12> AUX_PINS = {
    /* LoRa radio (SX127x) */
    GPIO_NUM_5,   // SS  (leave even if NC – harmless)
    GPIO_NUM_18,  // SCK / NSS
    GPIO_NUM_19,  // MISO
    GPIO_NUM_23,  // MOSI

    GPIO_NUM_14,  // RESET
    GPIO_NUM_26,  // DIO0
    GPIO_NUM_27,
    GPIO_NUM_33,  // DIO1
    GPIO_NUM_32,  // DIO2
    /* I²C battery gauge */
    GPIO_NUM_21,  // SDA
    GPIO_NUM_22,  // SCL
    GPIO_NUM_4    // MAX17048 ALRT
};

// Schedule TX every this many seconds
// Respect Fair Access Policy and Maximum Duty Cycle!
// https://www.thethingsnetwork.org/docs/lorawan/duty-cycle.html
// https://www.loratools.nl/#/airtime
const unsigned TX_INTERVAL = 3600;

void setup() {
    setCpuFrequencyMhz(40);
    WiFi.mode(WIFI_OFF);
    WiFi.disconnect(true);
    esp_wifi_stop();
    esp_bt_controller_disable();
    btStop();
    gpio_deep_sleep_hold_dis();
    gpio_hold_dis(static_cast<gpio_num_t>(VCC_ENA_PIN));
    for (gpio_num_t p : AUX_PINS) {
        gpio_hold_dis(p);
    }
    pinMode(VCC_ENA_PIN, OUTPUT);
    digitalWrite(VCC_ENA_PIN, HIGH);
    gpio_hold_dis((gpio_num_t)VCC_ENA_PIN);
    delay(100);
    pinMode(START_WEB_CONFIG_PIN, INPUT);
    Wire.begin(21, 22);
    delay(100);
    Serial.begin(115200);
    adc_power_acquire();
    randomSeed(analogRead(0));
    // Init and Wake the Lipo Cel:
    maxlipo.wake();
    maxlipo.sleep(false);

    if (maxlipo.begin()) {
        // Wake up the lipo gauge
        maxLipoFound = true;
        Serial.println("LIPO GAUGE FOUND");
    } else {
        Serial.println("No MAX14048 found");
    }
    FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
    lorawan_preferences_init();
    Serial.print("LMIC CONFIG Present: ");
    Serial.println(lorawanConfigPresent());
    startWebConfig = !digitalRead(START_WEB_CONFIG_PIN);
    Serial.print("Webconf status: ");
    Serial.println(startWebConfig);
    bool otaa_cfg = settings_has_key("ttn_otaa_config");
    Serial.print("otaa_config_done: ");
    Serial.println(otaa_cfg);
    Serial.print("Sleeping for: ");
    Serial.print(get_sleep_time_seconds());
    Serial.println(" seconds");

    if ((startWebConfig == true) || (!otaa_cfg)) {
        setCpuFrequencyMhz(80);
        WiFi.disconnect(false);
        resetLmic();
        startWebConf();
    }

    PrintLMICVersion();
    // LMIC init
    os_init();

    LMIC_reset();
    if (!lmic_init_needed()) {
        Serial.println(LMIC.seqnoUp);
        Serial.println("LMIC Data found");
        load_lmic();

        Serial.println(LMIC.seqnoUp);
    }

    LoraWANDebug(LMIC);

    // Start job (sending automatically starts OTAA too)
    LMIC_selectSubBand(1);

    do_send(&sendjob);
    entry = millis();
}

void loop() {
    static unsigned long lastPrintTime = 0;
    os_runloop_once();
    const bool timeCriticalJobs =
        os_queryTimeCriticalJobs(ms2osticksRound((TX_INTERVAL * 1000)));

    if (!timeCriticalJobs && enableSleep_ == true &&
        !(LMIC.opmode & OP_TXRXPEND)) {
        Serial.print(F("Can go sleep "));
        LoraWANPrintLMICOpmode();
        lmic_save();
        GoDeepSleep();
    } else if (lastPrintTime + 1000 < millis()) {
        Serial.print(F("Cannot sleep "));
        Serial.print(F("TimeCriticalJobs: "));
        Serial.print(timeCriticalJobs);
        Serial.print(" ");

        LoraWANPrintLMICOpmode();
        PrintRuntime();
        lastPrintTime = millis();
    }
    if (millis() - entry > 60000) {
        // ToDo: Delete LMIC config from nvram
        // so it joins the wan after next run
        resetLmic();
        GoDeepSleep();
    }
}

void PrintRuntime() {
    long seconds = millis() / 1000;
    Serial.print("Runtime: ");
    Serial.print(seconds);
    Serial.println(" seconds");
}

[[noreturn]] void GoDeepSleep() {
    // Turn off the lipo gauge:
    //
    unsigned long long sleepTime = get_sleep_time_seconds();
    Serial.println("Going to DeepSleep for: ");
    Serial.println(sleepTime);
    leds[0] = CRGB::Black;
    FastLED.show();

    WiFi.mode(WIFI_OFF);
    WiFi.disconnect(true);
    btStop();

    maxlipo.hibernate();
    maxlipo.enableSleep(true);
    maxlipo.sleep(true);
    LMIC_shutdown();
    Wire.end();
    SPI.end();

    for (gpio_num_t pin : AUX_PINS) {
        gpio_set_direction(pin, GPIO_MODE_INPUT);
        gpio_set_pull_mode(pin, GPIO_FLOATING);
        gpio_pullup_dis(pin);        // just in case
        gpio_pulldown_dis(pin);
        gpio_hold_en(pin);        // latch high-Z through sleep
    }

    /* -------- 2. Reset only the *always-on* GPIOs -------- */
    gpio_reset_pin(GPIO_NUM_0);
    gpio_reset_pin(GPIO_NUM_2);
    gpio_reset_pin(GPIO_NUM_12);
    gpio_reset_pin(GPIO_NUM_15);
    gpio_reset_pin(GPIO_NUM_25);
    /* AUX pins (14,26,27,32,33) are *not* reset here */

    gpio_reset_pin(GPIO_NUM_34);
    gpio_reset_pin(GPIO_NUM_35);
    gpio_reset_pin(GPIO_NUM_36);
    gpio_reset_pin(GPIO_NUM_37);
    gpio_reset_pin(GPIO_NUM_38);
    gpio_reset_pin(GPIO_NUM_39);

    adc_power_release();
    Serial.println(F("Go DeepSleep"));
    PrintRuntime();
    Serial.flush();

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
    save_runtime(millis() / 1000);

    digitalWrite(VCC_ENA_PIN, LOW);
    gpio_hold_en((gpio_num_t)VCC_ENA_PIN);
    delay(100);
    gpio_deep_sleep_hold_en();

    setCpuFrequencyMhz(20);
    esp_sleep_enable_timer_wakeup(sleepTime * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}
#else
// Minimal stubs for native build
int main() { return 0; }
#ifdef UNIT_TEST
#include "../../test/stubs/esp32_gpio.h"
lmic_t LMIC{};
#endif
#endif
