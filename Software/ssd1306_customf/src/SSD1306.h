#ifndef SSD1306_H
#define SSD1306_H

#include <Arduino.h>
#include <Wire.h>               // Pour I2C
#include <stdint.h>
#include <stddef.h>
#include <string.h>             // Pour memset, memcpy, etc.
#include "ssd1306_conf.h"
#include "SSD1306_fonts.h"

// =================== CONFIG ====================

#define SSD1306_WIDTH    128
#define SSD1306_HEIGHT   64
#define SSD1306_BUFFER_SIZE  (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

enum SSD1306_COLOR {
    Black = 0x00,
    White = 0x01
};


typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    bool Initialized;
    bool DisplayOn;
} SSD1306_t;

// =================== CLASS ====================

class SSD1306 {
public:
    SSD1306(uint8_t i2c_address);

    void Init();
    void Reset();
    void UpdateScreen();
    void Fill(SSD1306_COLOR color);
    void DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
    void DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
    void DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
    void FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
    void SetContrast(uint8_t value);
    void DisplayOn(bool on);
    bool IsDisplayOn();
    void SetCursor(uint8_t x, uint8_t y);
    void BatteryGauge(float vbat, uint8_t x, uint8_t y, uint8_t currentsquare);
    void Percentage(float percent);
    void DrawBitmap(uint8_t x, uint8_t y, const uint8_t* bitmap, uint8_t w, uint8_t h, SSD1306_COLOR color);
char WriteChar(char ch, const FontDef& Font, SSD1306_COLOR color);
char WriteString(const char* str, const FontDef& Font, SSD1306_COLOR color);



private:
    void WriteCommand(uint8_t cmd);
    void WriteData(const uint8_t* buffer, size_t buff_size);

private:
    uint8_t _i2c_address;
    uint8_t _buffer[SSD1306_BUFFER_SIZE];
    SSD1306_t _screen;
};

#endif
