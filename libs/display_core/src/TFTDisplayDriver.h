// TFT_eSPI sprite-backed display driver callbacks.
#pragma once

#include <TFT_eSPI.h>

#include "Display.h"

struct TFTDisplayDriverContext
{
  TFT_eSPI tft;
  TFT_eSprite sprite;

  TFTDisplayDriverContext() : tft(), sprite(&tft) {}
};

namespace TFTDisplayDriver
{
  DisplayDriverOps makeOps();
  DisplayConfig makeConfig(TFTDisplayDriverContext *context, uint16_t width, uint16_t height, uint8_t rotation, uint8_t colorDepth = 16);
}
