#include "Car.h"

#include "config.h"
#include "Display.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

namespace CarScreenNamespace
{

  void draw()
  {
    display.setTextDatum(TC_DATUM);
    display.setTextSize(3);
    display.setTextColor(TFT_WHITE);
    display.drawString("Car Control", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 20);
    display.setTextSize(2);
    display.drawString("(stub)", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 10);
  }

  void update()
  {
    if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
      screenManager.back();
  }

} // namespace CarScreenNamespace

const Screen2 CarScreen = {
    .name = "Car",
    .draw = CarScreenNamespace::draw,
    .update = CarScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_WHITE,
    .topBarTextColor = TFT_BLACK,
};
