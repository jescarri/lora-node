#include "lorawan.hpp"
#include "lorawan_settings.hpp"
#include <Adafruit_MAX1704X.h>

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

void LoraWANDebug(lmic_t lmic_check) {

  LoraWANPrintLMICOpmode();
  Serial.println("");
  Serial.println("-----");

  Serial.print(F("LMIC.seqnoUp = "));
  Serial.println(lmic_check.seqnoUp);

  Serial.print(F("LMIC.globalDutyRate = "));
  Serial.print(lmic_check.globalDutyRate);
  Serial.print(F(" osTicks, "));
  Serial.print(osticks2ms(lmic_check.globalDutyRate) / 1000);
  Serial.println(F(" sec"));

  Serial.print(F("LMIC.globalDutyAvail = "));
  Serial.print(lmic_check.globalDutyAvail);
  Serial.print(F(" osTicks, "));
  Serial.print(osticks2ms(lmic_check.globalDutyAvail) / 1000);
  Serial.println(F(" sec"));

  Serial.print(F("LMICbandplan_nextTx = "));
  Serial.print(LMICbandplan_nextTx(os_getTime()));
  Serial.print(F(" osTicks, "));
  Serial.print(osticks2ms(LMICbandplan_nextTx(os_getTime())) / 1000);
  Serial.println(F(" sec"));

  Serial.print(F("os_getTime = "));
  Serial.print(os_getTime());
  Serial.print(F(" osTicks, "));
  Serial.print(osticks2ms(os_getTime()) / 1000);
  Serial.println(F(" sec"));

  Serial.print(F("LMIC.txend = "));
  Serial.println(lmic_check.txend);
  Serial.print(F("LMIC.txChnl = "));
  Serial.println(lmic_check.txChnl);

  Serial.println("");
  Serial.println("");
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
      u4_t netid = 0;
      devaddr_t devaddr = 0;
      u1_t nwkKey[16];
      u1_t artKey[16];
      LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
      Serial.print("netid: ");
      Serial.println(netid, DEC);
      Serial.print("devaddr: ");
      Serial.println(devaddr, HEX);
      Serial.print("artKey: ");
      for (size_t i = 0; i < sizeof(artKey); ++i) {
        Serial.print(artKey[i], HEX);
      }
      Serial.println("");
      Serial.print("nwkKey: ");
      for (size_t i = 0; i < sizeof(nwkKey); ++i) {
        Serial.print(nwkKey[i], HEX);
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
    // setTxIndicatorsOn(false);
    // printEvent(timestamp, ev);
    // printFrameCounters();
    // Check if downlink was received
    if (LMIC.dataLen != 0 || LMIC.dataBeg != 0) {
      uint8_t fPort = 0;
      if (LMIC.txrxFlags & TXRX_PORT) {
        fPort = LMIC.frame[LMIC.dataBeg - 1];
      }
      // printDownlinkInfo();
      // processDownlink(timestamp, fPort, LMIC.frame + LMIC.dataBeg,
      // LMIC.dataLen);
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

void do_send(osjob_t *j) {
  lpp.reset();
  ReadSensors();
  lpp.addGenericSensor(0, sd.soilMoistureValue);
  lpp.addVoltage(1, sd.vBat);
  lpp.addPercentage(2, sd.soilMoisturePercentage);
  lpp.addSwitch(3, sd.lipoGaugeOk);
  lpp.addPercentage(4, sd.batPercent);
  lpp.addPercentage(5, sd.batRate);
  lpp.addGenericSensor(6, get_calibration_air_value());
  lpp.addGenericSensor(7, get_calibration_water_value());
  lpp.addGenericSensor(8, get_sleep_time_seconds());

  // Check if there is not a current TX/RX job running
  Serial.println("do_send");
  Serial.print("LMIC.opmode= ");
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
void os_getArtEui(u1_t *buf) {
  char char_app_eui[MAX_LORAWAN_CONF_CHAR_LEN];
  strcpy(char_app_eui, lorawan_preferences.getString("app_eui").c_str());
  u1_t app_eui[8];
  int c = 0;
  for (int i = 0; i < 16; i += 2) {
    char t[3];
    t[0] = char_app_eui[i];
    t[1] = char_app_eui[i + 1];
    t[2] = 0;
    app_eui[c] = strtoul(t, NULL, 16);
    c++;
  }
  Serial.print("app_eui: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(app_eui[i], HEX);
  }
  Serial.println("");
  memcpy_P(buf, app_eui, 8);
}
void os_getDevEui(u1_t *buf) {
  char char_dev_eui[MAX_LORAWAN_CONF_CHAR_LEN];
  strcpy(char_dev_eui, lorawan_preferences.getString("dev_eui").c_str());
  u1_t dev_eui[8];
  int c = 0;
  for (int i = 0; i < 16; i += 2) {
    char t[3];
    t[0] = char_dev_eui[i];
    t[1] = char_dev_eui[i + 1];
    t[2] = 0;
    dev_eui[c] = strtoul(t, NULL, 16);
    c++;
  }
  Serial.print("dev_eui: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(dev_eui[i], HEX);
  }
  Serial.println("");

  memcpy_P(buf, dev_eui, 8);
}
void os_getDevKey(u1_t *buf) {
  char char_app_key[MAX_LORAWAN_CONF_CHAR_LEN];
  strcpy(char_app_key, lorawan_preferences.getString("app_key").c_str());
  u1_t app_key[16];
  Serial.print("Read Data: ");
  Serial.println(char_app_key);
  int c = 0;
  for (int i = 0; i < 32; i += 2) {
    char t[3];
    t[0] = char_app_key[i];
    t[1] = char_app_key[i + 1];
    t[2] = 0;
    Serial.print(t);
    app_key[c] = strtoul(t, NULL, 16);
    c++;
  }
  Serial.println("");
  Serial.print("C Counter: ");
  Serial.println(c);
  for (int i = 0; i < 16; i++) {
    Serial.print(app_key[i], HEX);
  }
  Serial.println("");
  memcpy_P(buf, app_key, 16);
}

void ReadSensors() {
  sd.soilMoisturePercentage = 0;
  sd.soilMoistureValue = 0;
  sd.lipoGaugeOk = maxLipoFound;
  if (maxLipoFound == true) {
    sd.vBat = maxlipo.cellVoltage();
    sd.batPercent = maxlipo.cellPercent();
    sd.batRate = maxlipo.chargeRate();
    Serial.print("----ChargeRate: ");
    Serial.println(sd.batRate);
  }
  for (int i = 0; i < MAX_SENSOR_READ; i++) {
    float a = analogRead(SOIL_SENSOR_PIN);
    sd.soilMoistureValue += a;
    delay(10);
  }
  float t = sd.soilMoistureValue / MAX_SENSOR_READ;
  sd.soilMoistureValue = t;
  float x = map(sd.soilMoistureValue, get_calibration_air_value(),
                get_calibration_water_value(), 0, 100);
  sd.soilMoisturePercentage = abs(x);
  Serial.printf("X: %f", x);
  Serial.println("");
  Serial.printf("Moisture ADC: %f, Moisture Percentage: %f, vBat %f\n\n",
                sd.soilMoistureValue, sd.soilMoisturePercentage, sd.vBat);
}
