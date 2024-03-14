#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

class MQTTInfoScreen : public Screen
{
public:
  MQTTInfoScreen(String _name);

  void draw() override;
  void update() override;
};

MQTTInfoScreen::MQTTInfoScreen(String _name) : Screen(_name)
{
}

void MQTTInfoScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Mqtt Info");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);
}

void MQTTInfoScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }
}