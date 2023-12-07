#pragma once

#include <Arduino.h>
#include "driver/Display.h"

class ScreenUtils
{
public:
  void drawMenuItem(String name, int x, int y, bool selected, u16_t selectedColor = TFT_BLUE, u16_t unselectedColor = TFT_WHITE);
};

extern ScreenUtils screenUtils;