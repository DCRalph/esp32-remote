#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "IO/myespnow.h"

class EspnowSwitchScreen : public Screen
{
public:
  EspnowSwitchScreen(String _name);

  void draw() override;
  void update() override;
  void onEnter() override;

  bool lastState = false;
  int lastSendResult = -1;
};

EspnowSwitchScreen::EspnowSwitchScreen(String _name) : Screen(_name)
{
}

void EspnowSwitchScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Espnow Switch");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  char buffer[100];

  sprintf(buffer, "State: %s", lastState ? "ON" : "OFF");
  display.u8g2.drawStr(0, 30, buffer);

  sprintf(buffer, "Res: %s", lastSendResult == 0 ? "OK" : String(lastSendResult).c_str());
  display.u8g2.drawStr(0, 45, buffer);

  sprintf(buffer, "lastS: %d", myEspnow.lastStatus);
  display.u8g2.drawStr(0, 60, buffer);

  if (lastState != switchPin.read())
  {
    lastState = switchPin.read();

    lastSendResult = myEspnow.send(lastState ? "ON" : "OFF");
  }
}

void EspnowSwitchScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }
}

void EspnowSwitchScreen::onEnter()
{
  lastState = switchPin.read();
}