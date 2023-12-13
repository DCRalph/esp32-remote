#include "driver/ScreenUtils.h"



void ScreenUtils::drawMenuItem(String name, int x, int y, bool selected, u16_t selectedColor, u16_t unselectedColor)
{
  display.sprite.setTextSize(2);
  display.sprite.setTextDatum(TL_DATUM);

  if (selected)
    display.sprite.setTextColor(selectedColor);
  else
    display.sprite.setTextColor(unselectedColor);

  display.sprite.drawString(name, x, y * 20 + 70);
}

ScreenUtils screenUtils;