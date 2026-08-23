#pragma once

#include <Display.h>
#include <ScreenManager.h>

#include "config.h"
#include "IO/Battery.h"
#include "IO/GPIO.h"
#include "IO/U8g2DisplayDriver.h"
#include "Screens/Screens.h"

namespace
{
  void batteryScreenDraw()
  {
    display.setTextSize(U8G2_TEXT_TITLE);
    display.setTextColor(TFT_WHITE);
    display.setTextDatum(TL_DATUM);

    char buffer[32];

    snprintf(buffer, sizeof(buffer), "%.2fV. %d%%", batteryGetVoltage(), batteryGetPercentage());
    display.drawString(buffer, 0, 12);

    snprintf(buffer, sizeof(buffer), "%.2fV. %d%%", batteryGetVoltageSmooth(), batteryGetPercentageSmooth());
    display.drawString(buffer, 0, 30);

    snprintf(buffer, sizeof(buffer), "%d, %d, %d", MCPPowerGood.read(), MCPState1.read(), MCPState2.read());
    display.drawString(buffer, 0, 48);
  }

  void batteryScreenUpdate()
  {
    if (ClickButtonEnc.clicks == 1)
      screenManager.back();
  }
}

const Screen2 BatteryScreen2 = {
    .name = "Battery",
    .draw = batteryScreenDraw,
    .update = batteryScreenUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
