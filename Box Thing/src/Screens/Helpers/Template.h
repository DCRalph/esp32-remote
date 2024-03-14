#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

class templateScreen : public Screen
{
public:
  templateScreen(String _name);

  void draw() override;
  void update() override;
};

templateScreen::templateScreen(String _name) : Screen(_name)
{
}

void templateScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Template");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);
}

void templateScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }
}