// Minimal config for display library (no framework deps like Preferences/WiFi).
#pragma once

#include <Arduino.h>

#define TFT_ROT 1

#if TFT_ROT == 0 || TFT_ROT == 2
#define LCD_WIDTH  170
#define LCD_HEIGHT 320
#elif TFT_ROT == 1 || TFT_ROT == 3
#define LCD_WIDTH  320
#define LCD_HEIGHT 170
#endif
