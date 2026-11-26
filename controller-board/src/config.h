#pragma once

#include <Arduino.h>

// Panel after rotation. TFT_eSPI pins are build flags in platformio.ini.
constexpr uint16_t LCD_WIDTH = 320;
constexpr uint16_t LCD_HEIGHT = 240;

// From bioreactor-controller src/config.h.
constexpr uint8_t LATCH_PWR_PIN = 5;          // high holds the power supply on
constexpr uint8_t POWER_BUTTON_PIN = 15;      // active high, pulled down
constexpr uint8_t BOOT_BUTTON_PIN = 0;        // the ESP32-S3 BOOT button, active low
constexpr uint8_t DISPLAY_BACKLIGHT_PIN = 38; // active level depends on hardware version

// The panel turned 180 degrees from the bioreactor's orientation.
constexpr bool ROTATE_180 = true;

// Hold the power button this long to switch off.
constexpr uint32_t POWER_OFF_HOLD_MS = 2000;

// loop() sleeps out what is left of each frame; touch is also sampled mid-frame.
constexpr uint32_t FRAME_INTERVAL_MS = 10;

// The bioreactor's XPT2046 filtering (IO/TFTDisplay.h), as TouchInput options.
namespace TouchFilter
{
  constexpr uint8_t kReleasePolls = 5;     // empty polls before a press counts as lifted
  constexpr uint16_t kStartPressure = 600; // raw z to begin a press; idle noise peaks near 200
  constexpr uint16_t kHoldPressure = 20;   // raw z to keep one going
  constexpr uint8_t kAgreePx = 20;         // two samples per poll must agree within this (px)
}
