#include "SSD1306.h"

SSD1306::SSD1306(uint8_t i2c_address) : _i2c_address(i2c_address) {
    _screen.CurrentX = 0;
    _screen.CurrentY = 0;
    _screen.DisplayOn = 0;
    _screen.Initialized = 0;
    memset(_buffer, 0, SSD1306_BUFFER_SIZE);
}

void SSD1306::Reset() {
    // Pas nécessaire avec Arduino Wire pour I2C
}

void SSD1306::WriteCommand(uint8_t cmd) {
    Wire.beginTransmission(_i2c_address);
    Wire.write(0x00);
    Wire.write(cmd);
    Wire.endTransmission();
}

void SSD1306::WriteData(const uint8_t* buffer, size_t buff_size) {
    Wire.beginTransmission(_i2c_address);
    Wire.write(0x40);
    size_t pos = 0;
    while (pos < buff_size) {
        size_t chunkSize = buff_size - pos;
        if (chunkSize > 16) chunkSize = 16;
        Wire.write(buffer + pos, chunkSize);
        pos += chunkSize;
    }
    Wire.endTransmission();
}

void SSD1306::Init() {
    Reset();
    delay(100);

    DisplayOn(false);

    WriteCommand(0x20);
    WriteCommand(0x00);

    WriteCommand(0xB0);

    WriteCommand(0xC8);

    WriteCommand(0x00);
    WriteCommand(0x10);

    WriteCommand(0x40);

    SetContrast(0xFF);

    WriteCommand(0xA1);

    WriteCommand(0xA6);

    WriteCommand(0xA8);
    WriteCommand(0x3F);

    WriteCommand(0xA4);

    WriteCommand(0xD3);
    WriteCommand(0x00);

    WriteCommand(0xD5);
    WriteCommand(0xF0);

    WriteCommand(0xD9);
    WriteCommand(0x22);

    WriteCommand(0xDA);
    WriteCommand(0x12);

    WriteCommand(0xDB);
    WriteCommand(0x20);

    WriteCommand(0x8D);
    WriteCommand(0x14);

    DisplayOn(true);

    Fill(Black);
    UpdateScreen();

    _screen.CurrentX = 0;
    _screen.CurrentY = 0;
    _screen.Initialized = 1;
}

void SSD1306::Fill(SSD1306_COLOR color) {
    memset(_buffer, (color == Black) ? 0x00 : 0xFF, SSD1306_BUFFER_SIZE);
}

void SSD1306::UpdateScreen() {
    for (uint8_t i = 0; i < SSD1306_HEIGHT / 8; i++) {
        WriteCommand(0xB0 + i);
        WriteCommand(0x00);
        WriteCommand(0x10);
        WriteData(&_buffer[SSD1306_WIDTH * i], SSD1306_WIDTH);
    }
}

void SSD1306::DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;

    if (color == White) {
        _buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        _buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

void SSD1306::DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    int16_t deltaX = abs(x2 - x1);
    int16_t deltaY = abs(y2 - y1);
    int16_t signX = (x1 < x2) ? 1 : -1;
    int16_t signY = (y1 < y2) ? 1 : -1;
    int16_t error = deltaX - deltaY;
    int16_t error2;

    DrawPixel(x2, y2, color);

    while ((x1 != x2) || (y1 != y2)) {
        DrawPixel(x1, y1, color);
        error2 = error * 2;
        if (error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }
        if (error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
}

void SSD1306::DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    DrawLine(x1, y1, x2, y1, color);
    DrawLine(x2, y1, x2, y2, color);
    DrawLine(x2, y2, x1, y2, color);
    DrawLine(x1, y2, x1, y1, color);
}

void SSD1306::FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    uint8_t xs = (x1 < x2) ? x1 : x2;
    uint8_t xe = (x1 > x2) ? x1 : x2;
    uint8_t ys = (y1 < y2) ? y1 : y2;
    uint8_t ye = (y1 > y2) ? y1 : y2;

    for (uint8_t y = ys; y <= ye && y < SSD1306_HEIGHT; y++) {
        for (uint8_t x = xs; x <= xe && x < SSD1306_WIDTH; x++) {
            DrawPixel(x, y, color);
        }
    }
}

void SSD1306::SetContrast(uint8_t value) {
    WriteCommand(0x81);
    WriteCommand(value);
}

void SSD1306::DisplayOn(bool on) {
    WriteCommand(on ? 0xAF : 0xAE);
    _screen.DisplayOn = on;
}

bool SSD1306::IsDisplayOn() {
    return _screen.DisplayOn;
}

void SSD1306::SetCursor(uint8_t x, uint8_t y) {
    _screen.CurrentX = x;
    _screen.CurrentY = y;
}

void SSD1306::BatteryGauge(float vbat, uint8_t x, uint8_t y, uint8_t currentsquare) {
    DrawLine(x + 15, y + 1, x + 15, y + 5, White);
    DrawLine(x + 16, y + 1, x + 16, y + 5, White);
    DrawRectangle(x, y, x + 14, y + 6, White);

    int segments = 0;
    if (vbat >= 4.0) segments = 5;
    else if (vbat >= 3.8) segments = 4;
    else if (vbat >= 3.6) segments = 3;
    else if (vbat >= 3.4) segments = 2;
    else if (vbat >= 3.2) segments = 1;
    else segments = 0;

    for (int i = 0; i < segments; i++) {
        int seg_x1 = x + 2 + i * 3;
        int seg_y1 = y + 2;
        int seg_x2 = seg_x1 + 1;
        int seg_y2 = y + 4;
        if (currentsquare == i + 1) {
            FillRectangle(seg_x1, seg_y1, seg_x2, seg_y2, White);
        } else {
            DrawRectangle(seg_x1, seg_y1, seg_x2, seg_y2, White);
        }
    }
}
