#include "RSSIMeter.h"

#include "config.h"
#include <WiFi.h>
#include "Display.h"
#include "IO/Buttons.h"
#include "ScreenManager.h"

namespace RSSIMeterNamespace
{

  void draw()
  {
    display.setTextDatum(MC_DATUM);

    display.setTextSize(5);
    display.setTextColor(TFT_CYAN);
    display.drawString("RSSI", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 30);

    display.setTextSize(7);
    display.setTextColor(TFT_WHITE);

    char buf[20];
    sprintf(buf, "%i", WiFi.RSSI());

    display.drawString(buf, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);
  }

  void update()
  {
    if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
      screenManager.back();
  }

} // namespace RSSIMeterNamespace

const Screen2 RSSIMeter = {
    .name = "RSSI",
    .draw = RSSIMeterNamespace::draw,
    .update = RSSIMeterNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
