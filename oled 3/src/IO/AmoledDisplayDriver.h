#pragma once

#include "config.h"
#include "driver/rm67162.h"
#include <TFT_eSPI.h>

#include "Display.h"

struct AmoledDisplayDriverContext
{
  TFT_eSPI tft;
  TFT_eSprite sprite;

  AmoledDisplayDriverContext() : tft(), sprite(&tft) {}
};

namespace AmoledDisplayDriver
{
  DisplayDriverOps makeOps();
  DisplayConfig makeConfig(AmoledDisplayDriverContext *context, uint16_t width, uint16_t height, uint8_t rotation, uint8_t colorDepth = 16);
}
