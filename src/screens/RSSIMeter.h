#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "driver/Screen.h"

#include "buttons.h"
#include <WiFi.h>

class RSSIMeter : public Screen
{
public:
  RSSIMeter(TFT_eSprite *_sprite, String _name) : Screen(_sprite, _name){};

  void draw() override;
  void update() override;
};

void RSSIMeter::draw()
{
  sprite->fillSprite(TFT_BLACK);
  drawTopBar(sprite, name);

  sprite->setTextDatum(MC_DATUM);

  sprite->setTextSize(5);
  sprite->setTextColor(TFT_CYAN);
  sprite->drawString("RSSI", LCD_WIDTH / 2, 150);

  sprite->setTextSize(7);
  sprite->setTextColor(TFT_WHITE);

  char buf[20];
  sprintf(buf, "%i", WiFi.RSSI());

  sprite->drawString(buf, LCD_WIDTH / 2, LCD_HEIGHT / 2);
}

void RSSIMeter::update()
{
  if (ClickButton0.clicks != 0 || ClickButton21.clicks != 0)
    currentScreen = 0;
}