#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

enum StartUpState
{
  StartUp,
  ConnectingWifi,
  ApStarted,
};

class StartUpScreen : public Screen
{
private:
  StartUpState state;

public:
  StartUpScreen(String _name);

  void draw() override;
  void update() override;

  void setState(StartUpState _state);
  StartUpState getState();
};

StartUpScreen::StartUpScreen(String _name) : Screen(_name)
{
  state = StartUpState::StartUp;
}

void StartUpScreen::setState(StartUpState _state)
{
  state = _state;
}

StartUpState StartUpScreen::getState()
{
  return state;
}

void StartUpScreen::draw()
{

  switch (state)
  {
  case StartUpState::StartUp:
    display.u8g2.setFont(u8g2_font_logisoso16_tr);
    display.u8g2.setDrawColor(1);

    display.drawCenteredText(30, "Starting up...");

    break;

  case StartUpState::ConnectingWifi:
    display.u8g2.setFont(u8g2_font_logisoso16_tr);
    display.u8g2.setDrawColor(1);

    display.drawCenteredText(30, "Connecting to");
    display.drawCenteredText(48, "WiFi...");

    break;

  case StartUpState::ApStarted:
    display.u8g2.setFont(u8g2_font_logisoso16_tr);
    display.u8g2.setDrawColor(1);

    display.drawCenteredText(30, "AP Started");

    display.u8g2.setFont(u8g2_font_t0_11b_tf);
    display.u8g2.setDrawColor(1);

    display.drawCenteredText(64, "Press to continue...");

    break;

  default:
    break;
  }
}

void StartUpScreen::update()
{
  if (state == StartUpState::ApStarted)
  {
    if (ClickButtonEnc.clicks == -1)
    {
      screenManager.setScreen("Home");
    }
  }
}