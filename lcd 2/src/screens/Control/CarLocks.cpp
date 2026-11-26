#include "CarLocks.h"

#include "config.h"
#include "Display.h"
#include "IO/Buttons.h"
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
    if (btnUp.clicks() == 1)
      RemoteProtocol::CarRelays::lock().sendTo(car_addr, {0});

    if (btnDown.clicks() == -1)
      RemoteProtocol::CarRelays::lock().sendTo(car_addr, {1});

    if (btnDown.clicks() == 1)
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
