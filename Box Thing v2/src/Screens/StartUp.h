#pragma once

#include <Display.h>
#include <ScreenManager.h>

#include "config.h"
#include "IO/GPIO.h"
#include "IO/U8g2DisplayDriver.h"
#include "Screens/Screens.h"

enum class StartUpState
{
  StartUp,
  ConnectingWifi,
  ApStarted,
  EspNowStarted
};

namespace StartUp
{
  void setState(StartUpState state);
  StartUpState getState();
  void setStage(int stage);
}

namespace
{
  StartUpState gStartUpState = StartUpState::StartUp;
  int gStartUpStage = 0;

  void startUpDraw()
  {
    display.noTopBar();
    display.setTextColor(TFT_WHITE);
    display.setTextDatum(TC_DATUM);
    display.setTextSize(U8G2_TEXT_TITLE);

    const int centreX = display.width() / 2;

    switch (gStartUpState)
    {
    case StartUpState::StartUp:
      display.drawString("Starting up...", centreX, 14);
      display.drawString("Stage " + String(gStartUpStage), centreX, 38);
      break;

    case StartUpState::ConnectingWifi:
      display.drawString("Connecting to", centreX, 14);
      display.drawString("WiFi...", centreX, 32);
      break;

    case StartUpState::ApStarted:
      display.drawString("AP Started", centreX, 14);
      display.setTextSize(U8G2_TEXT_MONO12);
      display.drawString("Press to continue...", centreX, 55);
      break;

    case StartUpState::EspNowStarted:
      display.drawString("ESP-NOW Started", centreX, 14);
      break;
    }
  }

  void startUpUpdate()
  {
    if (gStartUpState == StartUpState::ApStarted && ClickButtonEnc.clicks == 1)
      screenManager.setScreen(&HomeScreen2);
  }
}

void StartUp::setState(StartUpState state)
{
  gStartUpState = state;
}

StartUpState StartUp::getState()
{
  return gStartUpState;
}

void StartUp::setStage(int stage)
{
  gStartUpStage = stage;
}

const Screen2 StartUpScreen2 = {
    .name = "Start Up",
    .draw = startUpDraw,
    .update = startUpUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
