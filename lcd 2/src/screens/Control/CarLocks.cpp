#include "CarLocks.h"

#include "config.h"
#include "Display.h"
#include "IO/Buttons.h"
#include "Wireless.h"
#include "ScreenManager.h"

namespace CarLocksScreenNamespace
{

  void draw()
  {
    display.noTopBar();

    display.setTextDatum(TL_DATUM);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(4);

    display.setTextDatum(TR_DATUM);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(3);
    display.drawString("Lock ->", LCD_WIDTH - 2, 2);

    display.setTextDatum(BR_DATUM);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(3);
    display.drawString("(exit) Unlock ->", LCD_WIDTH - 2, LCD_HEIGHT - 2);
  }

  void update()
  {
    if (ClickButtonUP.clicks == 1)
    {
      TransportPacket p;
      p.type = 0x10;
      p.len = 1;
      p.data[0] = 0;

      wireless.send(&p, car_addr);
    }

    if (ClickButtonDOWN.clicks == -1)
    {
      TransportPacket p;
      p.type = 0x10;
      p.len = 1;
      p.data[0] = 1;

      wireless.send(&p, car_addr);
    }

    if (ClickButtonDOWN.clicks == 1)
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
