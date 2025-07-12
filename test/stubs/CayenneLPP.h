#ifndef CAYENNELPP_H
#define CAYENNELPP_H

#include <Arduino.h>

class CayenneLPP {
public:
    CayenneLPP(uint8_t size) {}
    void reset() {}
    uint8_t getSize() const { return 0; }
    uint8_t* getBuffer() { return nullptr; }
    void addGenericSensor(uint8_t channel, float value) {}
    void addDigitalInput(uint8_t channel, uint8_t value) {}
    void addDigitalOutput(uint8_t channel, uint8_t value) {}
    void addAnalogInput(uint8_t channel, float value) {}
    void addAnalogOutput(uint8_t channel, float value) {}
    void addLuminosity(uint8_t channel, uint16_t lux) {}
    void addPresence(uint8_t channel, uint8_t value) {}
    void addTemperature(uint8_t channel, float celsius) {}
    void addRelativeHumidity(uint8_t channel, float humidity) {}
    void addAccelerometer(uint8_t channel, float x, float y, float z) {}
    void addBarometricPressure(uint8_t channel, float hpa) {}
    void addGyrometer(uint8_t channel, float x, float y, float z) {}
    void addGPS(uint8_t channel, float latitude, float longitude, float meters) {}
    void addVoltage(uint8_t channel, float voltage) {}
    void addCurrent(uint8_t channel, float current) {}
    void addFrequency(uint8_t channel, uint32_t freq) {}
    void addPercentage(uint8_t channel, uint8_t percentage) {}
    void addAltitude(uint8_t channel, float meters) {}
    void addPower(uint8_t channel, uint32_t watts) {}
    void addDistance(uint8_t channel, float meters) {}
    void addEnergy(uint8_t channel, float kwh) {}
    void addDirection(uint8_t channel, float degrees) {}
    void addSwitch(uint8_t channel, uint8_t value) {}
    void addConcentration(uint8_t channel, uint16_t ppm) {}
    void addColour(uint8_t channel, uint8_t r, uint8_t g, uint8_t b) {}
    void addSoilMoisture(uint8_t channel, uint8_t moisture) {}
    void addSoilConductivity(uint8_t channel, uint16_t conductivity) {}
    void addPH(uint8_t channel, float ph) {}
    void addORP(uint8_t channel, uint16_t orp) {}
    void addDissolvedOxygen(uint8_t channel, float dissolvedOxygen) {}
    void addTurbidity(uint8_t channel, uint16_t turbidity) {}
    void addBatteryLevel(uint8_t channel, uint8_t level) {}
    void addWaterLevel(uint8_t channel, float level) {}
    void addAirFlow(uint8_t channel, float flow) {}
    void addAirPressure(uint8_t channel, float pressure) {}
    void addAirQuality(uint8_t channel, uint16_t quality) {}
    void addAirTemperature(uint8_t channel, float temperature) {}
    void addAirHumidity(uint8_t channel, float humidity) {}
    void addAirCO2(uint8_t channel, uint16_t co2) {}
    void addAirTVOC(uint8_t channel, uint16_t tvoc) {}
    void addAirPM25(uint8_t channel, uint16_t pm25) {}
    void addAirPM10(uint8_t channel, uint16_t pm10) {}
    void addAirOzone(uint8_t channel, float ozone) {}
    void addAirNO2(uint8_t channel, float no2) {}
    void addAirCO(uint8_t channel, float co) {}
    void addAirSO2(uint8_t channel, float so2) {}
    void addAirNH3(uint8_t channel, float nh3) {}
    void addAirH2S(uint8_t channel, float h2s) {}
    void addAirCl2(uint8_t channel, float cl2) {}
    void addAirHCl(uint8_t channel, float hcl) {}
    void addAirHCN(uint8_t channel, float hcn) {}
    void addAirH2(uint8_t channel, float h2) {}
    void addAirCH4(uint8_t channel, float ch4) {}
    void addAirC2H5OH(uint8_t channel, float c2h5oh) {}
    void addAirC3H8(uint8_t channel, float c3h8) {}
    void addAirC4H10(uint8_t channel, float c4h10) {}
    void addAirC5H12(uint8_t channel, float c5h12) {}
    void addAirC6H6(uint8_t channel, float c6h6) {}
    void addAirC7H8(uint8_t channel, float c7h8) {}
    void addAirC8H10(uint8_t channel, float c8h10) {}
    void addAirC9H20(uint8_t channel, float c9h20) {}
    void addAirC10H22(uint8_t channel, float c10h22) {}
    void addAirH2O2(uint8_t channel, float h2o2) {}
    void addAirO2(uint8_t channel, float o2) {}
    void addAirO3(uint8_t channel, float o3) {}
    void addAirNO(uint8_t channel, float no) {}
    void addAirNOx(uint8_t channel, float nox) {}
    void addAirCO2_ppm(uint8_t channel, uint16_t co2_ppm) {}
    void addAirTVOC_ppb(uint8_t channel, uint16_t tvoc_ppb) {}
    void addAirPM25_ugm3(uint8_t channel, uint16_t pm25_ugm3) {}
    void addAirPM10_ugm3(uint8_t channel, uint16_t pm10_ugm3) {}
    void addAirOzone_ppb(uint8_t channel, float ozone_ppb) {}
    void addAirNO2_ppb(uint8_t channel, float no2_ppb) {}
    void addAirCO_ppm(uint8_t channel, float co_ppm) {}
    void addAirSO2_ppb(uint8_t channel, float so2_ppb) {}
    void addAirNH3_ppb(uint8_t channel, float nh3_ppb) {}
    void addAirH2S_ppb(uint8_t channel, float h2s_ppb) {}
    void addAirCl2_ppb(uint8_t channel, float cl2_ppb) {}
    void addAirHCl_ppb(uint8_t channel, float hcl_ppb) {}
    void addAirHCN_ppb(uint8_t channel, float hcn_ppb) {}
    void addAirH2_ppm(uint8_t channel, float h2_ppm) {}
    void addAirCH4_ppm(uint8_t channel, float ch4_ppm) {}
    void addAirC2H5OH_ppm(uint8_t channel, float c2h5oh_ppm) {}
    void addAirC3H8_ppm(uint8_t channel, float c3h8_ppm) {}
    void addAirC4H10_ppm(uint8_t channel, float c4h10_ppm) {}
    void addAirC5H12_ppm(uint8_t channel, float c5h12_ppm) {}
    void addAirC6H6_ppm(uint8_t channel, float c6h6_ppm) {}
    void addAirC7H8_ppm(uint8_t channel, float c7h8_ppm) {}
    void addAirC8H10_ppm(uint8_t channel, float c8h10_ppm) {}
    void addAirC9H20_ppm(uint8_t channel, float c9h20_ppm) {}
    void addAirC10H22_ppm(uint8_t channel, float c10h22_ppm) {}
    void addAirH2O2_ppm(uint8_t channel, float h2o2_ppm) {}
    void addAirO2_percent(uint8_t channel, float o2_percent) {}
    void addAirO3_ppb(uint8_t channel, float o3_ppb) {}
    void addAirNO_ppb(uint8_t channel, float no_ppb) {}
    void addAirNOx_ppb(uint8_t channel, float nox_ppb) {}
};

#endif // CAYENNELPP_H
