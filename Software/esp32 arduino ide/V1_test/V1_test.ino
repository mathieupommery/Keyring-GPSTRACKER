#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HardwareSerial.h>
#include "ESP32_WS2812_Lib.h"
#include <Wire.h>
#include "SSD1306.h"

// --- OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define GPS_RX_PIN 17
#define GPS_TX_PIN 16

#define MY_SDA 22
#define MY_SCL 23

#define B1_PIN 21
#define B2_PIN 20
#define LED_PIN 19

#define FLASH_CS 18

#define GPIO_SUP_2 15
#define GPIO_SUP_1 14

#define FLASH_MOSI 7
#define FLASH_MISO 2
#define FLASH_HOLD 4
#define FLASH_WP 5

#define KILL_PIN 3
#define VBAT_ADC 0
#define INT_POWER_PIN 1


#define LEDS_COUNT 1
#define CHANNEL    0
ESP32_WS2812 strip = ESP32_WS2812(LEDS_COUNT, LED_PIN, CHANNEL, TYPE_GRB);
SSD1306 display;
// --- Setup ---
void setup() {
  Serial.begin(115200);
    strip.begin();
  strip.setBrightness(20);
  Wire.begin(MY_SDA, MY_SCL);
  display.Init();
  display.Fill(Black);
  display.UpdateScreen();
  display.DrawRectangle(40, 40, 50, 50, White);
  display.UpdateScreen();
}

// --- Loop ---
void loop() {
  // Rien ici, tout est dans les tâches
    for (int j = 0; j < 255; j += 2) {
    for (int i = 0; i < strip.getLedCount(); i++) {
      strip.setLedColorData(i, strip.Wheel((i * 256 / strip.getLedCount() + j) & 255));
    }
    strip.show();
    delay(10);
  }
}
