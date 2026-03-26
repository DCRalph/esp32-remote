#include "batteryScreen.h"

#include "config.h"
#include "Display.h"
#include "IO/Buttons.h"
#include "IO/Battery.h"
#include "ScreenManager.h"

namespace BatteryScreenNamespace
{

  void draw()
  {
    display.setTextDatum(TL_DATUM);

    int percent = battery.getPercentageI();
    percent = constrain(percent, 0, 100);
    int height = map(percent, 0, 100, 0, 121);

    u16_t color = TFT_GREEN;
    if (percent < 20)
      color = TFT_RED;
    else if (percent < 50)
      color = TFT_ORANGE;

    display.setTextSize(4);
    display.setTextColor(TFT_GREEN);
    display.drawString("Battery", 10, 30);

    display.drawRoundRect((LCD_WIDTH / 4 * 3) - 25, 36, 50, 125, 5, TFT_WHITE);
    display.fillRect((LCD_WIDTH / 4 * 3) - 5, 29, 10, 7, TFT_WHITE);

    display.fillRoundRect((LCD_WIDTH / 4 * 3) - 23, 36 + 2 + (121 - height), 46, height, 5, color);

    display.setTextSize(4);
    display.setTextColor(TFT_WHITE);

    char buf[10];

    sprintf(buf, "%.2fV", battery.getVoltage());
    display.drawString(buf, 10, 80);

    sprintf(buf, "%i%%", battery.getPercentageI());
    display.drawString(buf, 10, 120);
  }

  void update()
  {
    if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
      screenManager.back();
  }

} // namespace BatteryScreenNamespace

const Screen2 BatteryScreen = {
    .name = "Battery",
    .draw = BatteryScreenNamespace::draw,
    .update = BatteryScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
