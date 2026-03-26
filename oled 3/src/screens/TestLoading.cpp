#include "TestLoading.h"

#include "config.h"
#include "Display.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

namespace TestLoadingScreenNamespace
{

  void draw()
  {
    const int cx = LCD_WIDTH / 2;
    const int cy = LCD_HEIGHT / 2 + 10;
    const int outerR = 40;
    const int innerR = 26;

    unsigned long now = millis();
    int base = (now % 1000UL) * 360 / 1000UL;

    const int sweep = 90;

    int startA = base % 360;
    int endA = (base + sweep) % 360;
    display.drawSmoothArc(cx, cy, outerR, innerR, startA, endA, TFT_CYAN, TFT_BLACK, true);

    display.setTextDatum(TC_DATUM);
    display.setTextSize(2);
    display.setTextColor(TFT_WHITE);
    display.drawString("Loading...", LCD_WIDTH / 2, cy + outerR + 20);
  }

  void update()
  {
    if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
      screenManager.back();
  }

  void onEnter()
  {
  }

} // namespace TestLoadingScreenNamespace

const Screen2 TestLoadingScreen = {
    .name = "Test Loading",
    .draw = TestLoadingScreenNamespace::draw,
    .update = TestLoadingScreenNamespace::update,
    .onEnter = TestLoadingScreenNamespace::onEnter,
    .onExit = nullptr,
    .topBarColor = TFT_DARKGREY,
    .topBarTextColor = TFT_WHITE,
};
