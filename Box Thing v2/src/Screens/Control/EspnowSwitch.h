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
  int gEspnowSwitchState = 0;
  unsigned long gEspnowSwitchLastSend = 0;

  /** Send a car-lock command (type 11) to the paired remote. */
  void espnowSwitchSendLock(uint8_t value)
  {
    TransportPacket p{};
    p.type = 11;
    p.len = 1;
    p.data[0] = value;

    wireless.send(&p, remote_addr);
    gEspnowSwitchLastSend = millis();
  }

  void espnowSwitchDraw()
  {
    display.setTextSize(U8G2_TEXT_BAR);
    display.setTextColor(TFT_WHITE);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Res: %s",
             wireless.getLastStatus() == ESP_NOW_SEND_SUCCESS ? "OK" : "Fail");

    display.setTextDatum(TL_DATUM);
    display.drawString(buffer, 0, 16);

    display.setTextDatum(TC_DATUM);
    display.drawString(gEspnowSwitchState == 1   ? "Locking"
                       : gEspnowSwitchState == 2 ? "Unlocking"
                                                 : "Idle",
                       display.width() / 2, 40);
  }

  void espnowSwitchUpdate()
  {
    if (ClickButtonEnc.clicks == 1)
      screenManager.back();

    if (ClickButtonEnc.clicks == 2)
    {
      espnowSwitchSendLock(0); // lock
      gEspnowSwitchState = 1;
    }

    if (ClickButtonEnc.clicks == 3)
    {
      espnowSwitchSendLock(1); // unlock
      gEspnowSwitchState = 2;
    }

    if (gEspnowSwitchState > 0 && millis() - gEspnowSwitchLastSend > 1000)
      gEspnowSwitchState = 0;
  }
}

const Screen2 EspnowSwitchScreen2 = {
    .name = "ESPNOW",
    .draw = espnowSwitchDraw,
    .update = espnowSwitchUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
