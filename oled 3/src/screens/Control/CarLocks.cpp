#include "CarLocks.h"

#include "config.h"
#include "Display.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

namespace CarLocksScreenNamespace
{

  void draw()
  {
    display.setTextDatum(TC_DATUM);
    display.setTextSize(3);
    display.setTextColor(TFT_WHITE);
    display.drawString("Car Locks", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 20);
    display.setTextSize(2);
    display.drawString("(stub)", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 10);
  }

  void update()
  {
    if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
      screenManager.back();
  }

} // namespace CarLocksScreenNamespace

const Screen2 CarLocksScreen = {
    .name = "CarLocks",
    .draw = CarLocksScreenNamespace::draw,
    .update = CarLocksScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
