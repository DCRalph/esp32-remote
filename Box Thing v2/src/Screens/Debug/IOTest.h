#pragma once

#include <Display.h>
#include <ScreenManager.h>

#include "config.h"
#include "IO/GPIO.h"
#include "IO/BoxThingDisplay.h"
#include "Screens/Screens.h"

namespace
{
  int gIoTestLastClicks = 0;

  void ioTestDraw()
  {
    if (ClickButtonEnc.clicks != 0)
      gIoTestLastClicks = ClickButtonEnc.clicks;

    display.setTextSize(U8G2_TEXT_TITLE);
    display.setTextColor(Color::White);
    display.setTextDatum(Datum::TopLeft);

    char buffer[64];

    snprintf(buffer, sizeof(buffer), "Enc: %d:%d", (int)encoder.getCount(), (int)encoderDetents());
    display.drawString(buffer, 0, 12);

    snprintf(buffer, sizeof(buffer), "Btn: %d : %d", ClickButtonEnc.depressed, gIoTestLastClicks);
    display.drawString(buffer, 0, 30);
  }

  void ioTestUpdate()
  {
    if (ClickButtonEnc.clicks == 1)
      screenManager.back();

    if (ClickButtonEnc.clicks == 3)
      encoderReset();
  }
}

const Screen2 IOTestScreen2 = {
    .name = "IO Test",
    .draw = ioTestDraw,
    .update = ioTestUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
