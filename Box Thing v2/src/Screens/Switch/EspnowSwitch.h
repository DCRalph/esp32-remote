#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
// #include "IO/myespnow.h"
#include "IO/Wireless.h"

class EspnowSwitchScreen : public Screen
{
public:
  EspnowSwitchScreen(String _name);

  void draw() override;
  void update() override;
  void onEnter() override;

  bool lastState = false;
  int lastSendResult = -1;

  // 74:4d:bd:7b:93:6c
  uint8_t car_addr[6] = {0x74, 0x4d, 0xbd, 0x7b, 0x93, 0x6c};
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

  sprintf(buffer, "lastS: %d", wireless.lastStatus);
  display.u8g2.drawStr(0, 60, buffer);

  if (lastState != switchPin.read())
  {
    lastState = switchPin.read();

    // lastSendResult = myEspnow.send(lastState ? "ON" : "OFF");
    packet p;
    p.type = 1;
    p.len = 1;
    p.data[0] = lastState ? 1 : 0;
    lastSendResult = wireless.send(&p, car_addr);
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