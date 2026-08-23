// Starting point for a new screen.
//
// Copy this file, rename the symbols, declare the Screen2 in Screens/Screens.h,
// and include it from main.cpp. Screen headers are included exactly once, so
// definitions may live here rather than in a .cpp.
#pragma once

#include <Display.h>
#include <ScreenManager.h>

#include "config.h"
#include "IO/GPIO.h"
#include "IO/TopBar.h"
#include "IO/U8g2DisplayDriver.h"
#include "Screens/Screens.h"

namespace
{
  void templateDraw()
  {
    display.setTextSize(U8G2_TEXT_MONO12);
    display.setTextColor(TFT_WHITE);
    display.setTextDatum(TL_DATUM);
    display.drawString("Template", 0, TopBar::kHeight);
  }

  void templateUpdate()
  {
    if (ClickButtonEnc.clicks == 1)
      screenManager.back();
  }
}

const Screen2 TemplateScreen2 = {
    .name = "Template",
    .draw = templateDraw,
    .update = templateUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
