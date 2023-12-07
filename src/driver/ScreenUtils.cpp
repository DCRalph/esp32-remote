#include "driver/ScreenUtils.h"



void ScreenUtils::drawMenuItem(String name, int x, int y, bool selected, u16_t selectedColor, u16_t unselectedColor)
{
  display.sprite.setTextSize(3);
  display.sprite.setTextDatum(TL_DATUM);

  if (selected)
    display.sprite.setTextColor(selectedColor);
  else
    display.sprite.setTextColor(unselectedColor);

  display.sprite.drawString(name, x, y * 35 + 120);
}

ScreenUtils screenUtils;