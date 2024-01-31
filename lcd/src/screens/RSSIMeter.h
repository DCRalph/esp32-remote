#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "Buttons.h"
#include <WiFi.h>

class RSSIMeter : public Screen
{
public:
  RSSIMeter(String _name, String _id) : Screen(_name, _id){};

  void draw() override;
  void update() override;
};

void RSSIMeter::draw()
{
  display.sprite.setTextDatum(MC_DATUM);

  display.sprite.setTextSize(5);
  display.sprite.setTextColor(TFT_CYAN);
  display.sprite.drawString("RSSI", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 30);

  display.sprite.setTextSize(7);
  display.sprite.setTextColor(TFT_WHITE);

  char buf[20];
  sprintf(buf, "%i", WiFi.RSSI());

  display.sprite.drawString(buf, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);
}

void RSSIMeter::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.setScreen("settings");
}