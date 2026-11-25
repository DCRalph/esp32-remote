#pragma once

#include <Arduino.h>
#include <Preferences.h>

extern Preferences preferences;
/** Required by dcr_netLink: it declares this extern and latches it on first join. */
extern bool wifiConnectedAtSomePoint;

#define BTN_UP_PIN 14
#define BTN_DOWN_PIN 0
#define POWER_ON_PIN 15 // LCD and battery rail; low powers them down

#define BATT_MAX 4.2
#define BATT_MIN 3.0

// T-Display S3 built-in LCD, landscape.
#define LCD_ROTATION 1
#define LCD_WIDTH 320
#define LCD_HEIGHT 170

// ILI9341 touch panel, landscape.
#define TOUCH_WIDTH 320
#define TOUCH_HEIGHT 240

// ILI9341 + XPT2046 on one SPI bus (FSPI). Touch shares MOSI/SCLK/MISO.
// Not PIN_* names: TFT_eSPI's setup headers define PIN_MOSI and friends.
namespace TouchPanelPins
{
  constexpr int8_t kCS = 10;
  constexpr int8_t kRST = 3;
  constexpr int8_t kDC = 2;
  constexpr int8_t kMOSI = 11;
  constexpr int8_t kSCLK = 12;
  constexpr int8_t kMISO = 13;
  constexpr int8_t kTouchCS = 1;
}

// dcr_timeProfiler keys for each display's render(): calls per second and duration.
#define PROFILE_LCD "lcd"
#define PROFILE_TOUCH "touch"

void initConfig();
