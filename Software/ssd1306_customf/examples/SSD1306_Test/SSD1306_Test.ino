#include <SSD1306.h>
#include <Wire.h>

void setup() {
  ssd1306_Init(8, 9); // Modifier ici les pins SDA/SCL si besoin
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString((char*)"Bonjour ESP32!", Font_6x8, White);
  ssd1306_UpdateScreen();
}

void loop() {}
