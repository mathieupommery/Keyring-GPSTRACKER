#ifndef SSD1306_H
#define SSD1306_H

#include <Arduino.h>
#include <Wire.h>

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

typedef enum {
    Black = 0,
    White = 1,
} SSD1306_COLOR;

typedef struct {
    uint8_t CurrentX;
    uint8_t CurrentY;
    uint8_t DisplayOn;
    uint8_t Initialized;
} SSD1306_t;

class SSD1306 {
public:
    SSD1306(uint8_t i2c_address = 0x3C);

    void Init();
    void Fill(SSD1306_COLOR color);
    void UpdateScreen();
    void DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
    void DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
    void DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
    void FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
    void SetContrast(uint8_t value);
    void DisplayOn(bool on);
    bool IsDisplayOn();
    void SetCursor(uint8_t x, uint8_t y);

    void BatteryGauge(float vbat, uint8_t x, uint8_t y, uint8_t currentsquare);

private:
    uint8_t _i2c_address;
    uint8_t _buffer[SSD1306_BUFFER_SIZE];
    SSD1306_t _screen;

    void WriteCommand(uint8_t cmd);
    void WriteData(const uint8_t* buffer, size_t buff_size);
    void Reset();
};

#endif
