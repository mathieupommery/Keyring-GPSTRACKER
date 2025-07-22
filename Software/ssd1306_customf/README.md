# SSD1306_Custom

Portage Arduino d'une librairie SSD1306 initialement développée pour STM32.

## Utilisation

```cpp
#include <SSD1306.h>

void setup() {
  ssd1306_Init(21, 22); // Exemple pour ESP32
  ssd1306_WriteString((char*)"Hello!", Font_6x8, White);
  ssd1306_UpdateScreen();
}

void loop() {}
```
