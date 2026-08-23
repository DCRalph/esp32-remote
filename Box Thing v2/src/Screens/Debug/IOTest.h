#pragma once

#include <Display.h>
#include <ScreenManager.h>

#include "config.h"
#include "IO/GPIO.h"
#include "IO/U8g2DisplayDriver.h"
#include "Screens/Screens.h"

namespace
{
  int gIoTestLastClicks = 0;

  void ioTestDraw()
  {
    if (ClickButtonEnc.clicks != 0)
      gIoTestLastClicks = ClickButtonEnc.clicks;

    display.setTextSize(U8G2_TEXT_TITLE);
    display.setTextColor(TFT_WHITE);
    display.setTextDatum(TL_DATUM);

    char buffer[64];

    snprintf(buffer, sizeof(buffer), "Enc: %d:%d", (int)encoder.getCount(), (int)encoderGetCount());
    display.drawString(buffer, 0, 12);

    snprintf(buffer, sizeof(buffer), "Btn: %d : %d", ClickButtonEnc.depressed, gIoTestLastClicks);
    display.drawString(buffer, 0, 30);
  }

  void ioTestUpdate()
  {
    if (ClickButtonEnc.clicks == 1)
      screenManager.back();

    if (ClickButtonEnc.clicks == 3)
      encoder.clearCount();
  }
}

const Screen2 IOTestScreen2 = {
    .name = "IO Test",
    .draw = ioTestDraw,
    .update = ioTestUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
