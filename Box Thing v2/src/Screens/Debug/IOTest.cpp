
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
    if (knobButton.clicks() != 0)
      gIoTestLastClicks = knobButton.clicks();

    display.setTextSize(U8G2_TEXT_TITLE);
    display.setTextColor(Color::White);
    display.setTextDatum(Datum::TopLeft);

    char buffer[64];

    snprintf(buffer, sizeof(buffer), "Enc: %d:%d", (int)knob.rawCount(), (int)knob.position());
    display.drawString(buffer, 0, 12);

    snprintf(buffer, sizeof(buffer), "Btn: %d : %d", knobButton.pressed(), gIoTestLastClicks);
    display.drawString(buffer, 0, 30);
  }

  void ioTestUpdate()
  {
    if (knobButton.clicks() == 1)
      screenManager.back();

    if (knobButton.clicks() == 3)
      knob.reset();
  }
}

const Screen2 IOTestScreen2 = {
    .name = "IO Test",
    .draw = ioTestDraw,
    .update = ioTestUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
