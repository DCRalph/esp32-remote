#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
// #include "IO/myespnow.h"
#include "IO/Wireless.h"

class CarControlScreen : public Screen
{
public:
  CarControlScreen(String _name);

  void draw() override;
  void update() override;
  void onEnter() override;

  int state = 0;
  unsigned long lastSend = 0;
};

CarControlScreen::CarControlScreen(String _name) : Screen(_name)
{
}

void CarControlScreen::draw()
{

  char buffer[100];

  display.u8g2.setFont(u8g2_font_koleeko_tf);

  sprintf(buffer, "Res: %s", wireless.lastStatus == ESP_NOW_SEND_SUCCESS ? "OK" : "Fail");
  display.u8g2.drawStr(0, 24, buffer);

  sprintf(buffer, "%s", state == 1 ? "Locking" : state == 2 ? "Unlocking" : "Idle");
  display.drawCenteredText(48, buffer);

}

void CarControlScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }

  if (ClickButtonEnc.clicks == 2)
  {
    packet p;
    p.type = 11; // car locks
    p.len = 1;
    p.data[0] = 0; // lock

    wireless.send(&p, car_addr);

    state = 1;
    lastSend = millis();
  }

    if (ClickButtonEnc.clicks == 3)
  {
    packet p;
    p.type = 11; // car locks
    p.len = 1;
    p.data[0] = 1; // unlock

    wireless.send(&p, car_addr);

    state = 2;
    lastSend = millis();
  }

  if (state > 0 && millis() - lastSend > 1000)
  {
    state = 0;
  }
}

void CarControlScreen::onEnter()
{
}