#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

class HomeScreen : public Screen
{
public:
  HomeScreen(String _name) : Screen(_name){};

  void draw() override;
  void update() override;
};

void HomeScreen::draw()
{
  display.u8g2.setFont(u8g2_font_logisoso16_tf);

  char buffer[10];
  sprintf(buffer, "r: %d", encoder.encoder.getPosition());

  display.u8g2.drawStr(0, 32, buffer);
}

void HomeScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }
}