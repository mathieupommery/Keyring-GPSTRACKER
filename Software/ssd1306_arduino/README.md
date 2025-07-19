# SSD1306 Arduino Library

Simple library to drive SSD1306 OLED display (128x64) over I2C with Arduino.

## Usage

```cpp
#include <Wire.h>
#include "SSD1306.h"

SSD1306 display;

void setup() {
  Wire.begin();
  display.Init();
  display.Fill(Black);
  display.UpdateScreen();
  
  display.DrawRectangle(10, 10, 50, 30, White);
  display.UpdateScreen();
}

void loop() {
}
