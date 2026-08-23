#pragma once

#include <Display.h>
#include <ScreenManager.h>

#include "config.h"
#include "IO/GPIO.h"
#include "IO/U8g2DisplayDriver.h"
#include "Screens/Screens.h"

namespace
{
  enum class ShutdownState
  {
    Countdown,
    Warning,
    Shutdown
  };

  constexpr long kShutdownCountdownMs = 1500;

  ShutdownState gShutdownState = ShutdownState::Countdown;
  unsigned long gShutdownStartTime = 0;

  void shutdownDraw()
  {
    display.noTopBar();
    display.setTextColor(TFT_WHITE);
    display.setTextDatum(TC_DATUM);

    const int centreX = display.width() / 2;

    if (gShutdownState == ShutdownState::Countdown)
    {
      display.setTextSize(U8G2_TEXT_TITLE);
      display.drawString("Shutting down", centreX, 4);

      display.setTextSize(U8G2_TEXT_BAR);
      display.drawString("Press to cancel", centreX, 36);

      uint8_t progress = map(millis() - gShutdownStartTime, 0, kShutdownCountdownMs, 100, 0);
      progress = constrain(progress, 0, 100);

      display.drawRect(0, 48, 127, 16, TFT_WHITE);
      display.fillRect(2, 50, map(progress, 0, 100, 0, 123), 12, TFT_WHITE);
      return;
    }

    display.setTextSize(U8G2_TEXT_TITLE);
    display.drawString("Shutting down", centreX, 14);
  }

  void shutdownUpdate()
  {
    const unsigned long elapsed = millis() - gShutdownStartTime;

    switch (gShutdownState)
    {
    case ShutdownState::Countdown:
      if (elapsed > kShutdownCountdownMs)
        gShutdownState = ShutdownState::Warning;
      else if (ClickButtonEnc.clicks == 1)
        screenManager.back();
      break;

    case ShutdownState::Warning:
      if (elapsed > kShutdownCountdownMs + 500)
        gShutdownState = ShutdownState::Shutdown;
      break;

    case ShutdownState::Shutdown:
      debugI("Shutting down...");
      latch.Off();
      break;
    }
  }

  void shutdownOnEnter()
  {
    gShutdownState = ShutdownState::Countdown;
    gShutdownStartTime = millis();
  }
}

const Screen2 ShutdownScreen2 = {
    .name = "Shutdown",
    .draw = shutdownDraw,
    .update = shutdownUpdate,
    .onEnter = shutdownOnEnter,
    .onExit = nullptr,
};
