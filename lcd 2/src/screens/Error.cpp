#include "Error.h"

#include "config.h"
#include "Display.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

#include "Home.h"

namespace ErrorScreenNamespace
{

  void draw()
  {
    display.setTextDatum(TC_DATUM);

    display.setTextSize(5);
    display.setTextColor(TFT_RED);
    display.drawString("ERROR", LCD_WIDTH / 2, 50);

    display.setTextSize(2);
    display.setTextColor(TFT_WHITE);

    display.drawString("Press any button", LCD_WIDTH / 2, 100);
    display.drawString("to continue", LCD_WIDTH / 2, 130);
  }

  void update()
  {
    if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
      screenManager.setScreen(&HomeScreen);
  }

} // namespace ErrorScreenNamespace

const Screen2 ErrorScreen = {
    .name = "Error",
    .draw = ErrorScreenNamespace::draw,
    .update = ErrorScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
