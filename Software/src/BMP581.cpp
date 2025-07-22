#include "BMP581.h"
#include <math.h>

BMP581::BMP581(TwoWire& wire) {
    _wire = &wire;
}

uint8_t BMP581::writeRegister(uint8_t reg, uint8_t value) {
    _wire->beginTransmission(_writeAddr >> 1);
    _wire->write(reg);
    _wire->write(value);
    return _wire->endTransmission();
}

uint8_t BMP581::readRegisters(uint8_t reg, uint8_t* buffer, uint8_t length) {
    _wire->beginTransmission(_writeAddr >> 1);
    _wire->write(reg);
    if (_wire->endTransmission(false) != 0) return 1;

    if (_wire->requestFrom(_readAddr >> 1, length) != length) return 2;

    for (uint8_t i = 0; i < length; i++) {
        buffer[i] = _wire->read();
    }

    return 0;
}

uint8_t BMP581::init() {
    uint8_t status = 0;

    status |= writeRegister(BMP581_OSR_CONFIG, 0b01111011); // OSR
    status |= writeRegister(BMP581_ODR_CONFIG, 0b01101001); // ODR
    status |= writeRegister(BMP581_DSP_CONFIG, 0b00101011); // DSP
    status |= writeRegister(BMP581_DSP_CONFIG, 0b00010010); // DSP second write

    uint8_t dummy;
    status |= readRegisters(BMP581_OSR_EFF, &dummy, 1);

    return status;
}

uint8_t BMP581::read(BMP581_Data* data) {
    uint8_t buf[6];
    int32_t rawTemp = 0;
    int32_t rawPress = 0;

    uint8_t status = readRegisters(BMP581_TEMP_DATA_XLSB, buf, 6);
    if (status != 0) return status;

    rawTemp = ((int32_t)buf[2] << 16) | ((int32_t)buf[1] << 8) | buf[0];
    rawPress = ((int32_t)buf[5] << 16) | ((int32_t)buf[4] << 8) | buf[3];

    data->temperature = rawTemp / 65536.0f;
    data->pressure = rawPress / 64.0f;
    data->altitude = ((8.314f * 298.15f) / (9.80665f * 0.028964f)) * log(P0 / data->pressure);

    return 0;
}