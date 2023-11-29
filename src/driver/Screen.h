#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "driver/pins_config.h"

#include "battery.h"

int currentScreen = 0;

class Screen
{
public:
  TFT_eSprite *sprite;
  String name;

  Screen(TFT_eSprite *_sprite, String _name);

  virtual void draw();
  virtual void update();
};

Screen::Screen(TFT_eSprite *_sprite, String _name)
{
  sprite = _sprite;
  name = _name;
}

void Screen::draw()
{
  sprite->fillSprite(TFT_BLACK);

  sprite->fillSmoothRoundRect(0, 0, LCD_WIDTH, 50, 20, TFT_WHITE);

  sprite->setTextSize(3);
  sprite->setTextColor(TFT_BLACK);
  sprite->setTextDatum(ML_DATUM);

  char buf[20];

  sprintf(buf, "%s %i", name, millis());

  sprite->drawString(buf, 10, 25);
}

void Screen::update()
{
}

void drawTopBar(TFT_eSprite *sprite, String name)
{
  sprite->fillSmoothRoundRect(0, 0, LCD_WIDTH, 30, 20, TFT_ORANGE);

  sprite->setTextSize(3);
  sprite->setTextColor(TFT_BLACK);
  sprite->setTextDatum(ML_DATUM);

  sprite->drawString(name, 10, 15);
  char buf[20];
  sprintf(buf, "%.1fV", battery.getVoltage());
  sprite->setTextDatum(MR_DATUM);
  sprite->drawString(buf, LCD_WIDTH - 10, 15);
}

void drawMenuItem(TFT_eSprite *sprite, String name, int x, int y, bool selected, u16_t selectedColor = TFT_BLUE, u16_t unselectedColor = TFT_WHITE)
{
  sprite->setTextSize(3);
  sprite->setTextDatum(TL_DATUM);

  if (selected)
    sprite->setTextColor(selectedColor);
  else
    sprite->setTextColor(unselectedColor);

  sprite->drawString(name, x, y * 35 + 120);
}