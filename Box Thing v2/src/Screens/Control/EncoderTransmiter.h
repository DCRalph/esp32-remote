#pragma once

#include <Display.h>
#include <ScreenManager.h>
#include <Wireless.h>

#include "config.h"
#include "IO/GPIO.h"
#include "IO/U8g2DisplayDriver.h"
#include "Screens/Screens.h"

namespace
{
  uint64_t gEncoderTxLastSend = 0;
  int8_t gEncoderTxNextClicks = 0;

  void encoderTxDraw()
  {
    display.setTextSize(U8G2_TEXT_MONO10);
    display.setTextColor(TFT_WHITE);
    display.setTextDatum(TL_DATUM);

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

    const uint64_t encCount = encoderGetCount();

    WirelessFrame fp{};
    memcpy(fp.mac, wireless.broadcastAddress().data(), 6);
    fp.direction = PacketDirection::SEND;
    fp.packet.type = 0xa3;
    fp.packet.len = 9;

    memcpy(fp.packet.data, &encCount, 8);
    fp.packet.data[8] = gEncoderTxNextClicks;

    wireless.send(&fp);
    gEncoderTxNextClicks = 0;
  }

  void encoderTxOnEnter()
  {
    encoderClearCount();
  }
}

const Screen2 EncoderTransmiterScreen2 = {
    .name = "Encoder",
    .draw = encoderTxDraw,
    .update = encoderTxUpdate,
    .onEnter = encoderTxOnEnter,
    .onExit = nullptr,
};
