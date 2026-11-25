#pragma once

#include <Display.h>
#include <RemoteProtocol.h>
#include <ScreenManager.h>

#include "config.h"
#include "IO/GPIO.h"
#include "IO/BoxThingDisplay.h"
#include "Screens/Screens.h"

namespace
{
  uint64_t gEncoderTxLastSend = 0;
  int8_t gEncoderTxNextClicks = 0;

  void encoderTxDraw()
  {
    display.setTextSize(U8G2_TEXT_MONO10);
    display.setTextColor(Color::White);
    display.setTextDatum(Datum::TopLeft);

    display.drawString("Encoder Transmiter", 0, 22);
    display.drawString("Press encoder 2 times", 0, 42);
    display.drawString("and hold to exit", 0, 52);
  }

  void encoderTxUpdate()
  {
    if (ClickButtonEnc.clicks == -2)
      screenManager.back();

    if (ClickButtonEnc.clicks != 0)
      gEncoderTxNextClicks = ClickButtonEnc.clicks;

    if (millis() - gEncoderTxLastSend <= 10)
      return;

    gEncoderTxLastSend = millis();

    RemoteProtocol::Encoder::state().sendTo(wireless.broadcastAddress(), {encoderDetents(), gEncoderTxNextClicks});
    gEncoderTxNextClicks = 0;
  }

  void encoderTxOnEnter()
  {
    encoderReset();
  }
}

const Screen2 EncoderTransmiterScreen2 = {
    .name = "Encoder",
    .draw = encoderTxDraw,
    .update = encoderTxUpdate,
    .onEnter = encoderTxOnEnter,
    .onExit = nullptr,
};
