#ifndef BMP581_H
#define BMP581_H

#include <Arduino.h>
#include <Wire.h>

#define BMP581_WRITE_ADDR  (0x47 << 1) + 0
#define BMP581_READ_ADDR   (0x47 << 1) + 1

// Registres du BMP581
#define BMP581_OSR_CONFIG          0x36
#define BMP581_ODR_CONFIG          0x37
#define BMP581_DSP_CONFIG          0x30
#define BMP581_OSR_EFF             0x38

#define BMP581_TEMP_DATA_XLSB      0x1D
#define BMP581_PRESS_DATA_XLSB     0x20

struct BMP581_Data {
    float pressure;
    float temperature;
    float altitude;
};

class BMP581 {
public:
    BMP581(TwoWire& wire = Wire);

    uint8_t init();
    uint8_t read(BMP581_Data* data);

private:
    TwoWire* _wire;
    const uint8_t _writeAddr = BMP581_WRITE_ADDR;
    const uint8_t _readAddr  = BMP581_READ_ADDR;
    const double P0 = 101325.0;  // Pression au niveau de la mer

    uint8_t writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegisters(uint8_t reg, uint8_t* buffer, uint8_t length);
};

#endif // BMP581_H