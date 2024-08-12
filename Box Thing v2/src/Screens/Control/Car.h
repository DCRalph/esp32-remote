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
}

void CarControlScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }

  if (ClickButtonEnc.clicks == 2)
  {
    fullPacket p;
    p.direction = PacketDirection::SEND;
    memcpy(p.mac, car_addr, 6);
    p.p.type = CMD_DOOR_LOCK;
    p.p.len = 1;
    p.p.data[0] = 0x00;

    wireless.send(&p.p, p.mac);
  }

  if (ClickButtonEnc.clicks == 3)
  {
    fullPacket p;
    p.direction = PacketDirection::SEND;
    memcpy(p.mac, car_addr, 6);
    p.p.type = CMD_DOOR_LOCK;
    p.p.len = 1;
    p.p.data[0] = 0x01;

    wireless.send(&p.p, p.mac);
  }

  if (ClickButtonEnc.clicks == 4)
  {
    fullPacket p;
    p.direction = PacketDirection::SEND;
    memcpy(p.mac, car_addr, 6);
    p.p.type = CMD_RELAY_1_SET;
    p.p.len = 1;
    p.p.data[0] = 0x00;

    wireless.send(&p.p, p.mac);
  }

    if (ClickButtonEnc.clicks == 5)
  {
    fullPacket p;
    p.direction = PacketDirection::SEND;
    memcpy(p.mac, car_addr, 6);
    p.p.type = CMD_RELAY_1_SET;
    p.p.len = 1;
    p.p.data[0] = 0x01;

    wireless.send(&p.p, p.mac);
  }
}

void CarControlScreen::onEnter()
{
}