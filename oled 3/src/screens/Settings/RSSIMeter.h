#pragma once

#include "config.h"
#include <Display.h>
#include "IO/Buttons.h"

class RSSIMeter : public Screen
{
public:
  RSSIMeter(String _name) : Screen(_name) {};

  void draw() override;
  void update() override;
};

void RSSIMeter::draw()
{
  display.setTextDatum(MC_DATUM);

  display.setTextSize(5);
  display.setTextColor(TFT_CYAN);
  display.drawString("RSSI", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 30);

  display.setTextSize(7);
  display.setTextColor(TFT_WHITE);

  char buf[20];
  sprintf(buf, "%i", WiFi.RSSI());

  display.drawString(buf, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);
}

void RSSIMeter::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.back();
}