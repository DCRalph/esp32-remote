#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "Buttons.h"
#include <WiFi.h>

class EspnowScreen : public Screen
{
public:
  EspnowScreen(String _name) : Screen(_name){};

  void draw() override;
  void update() override;
};

void EspnowScreen::draw()
{
  display.sprite.setTextDatum(MC_DATUM);

  display.sprite.setTextSize(5);
  display.sprite.setTextColor(TFT_CYAN);
  display.sprite.drawString("ESPPNOW", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 30);
}

void EspnowScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.back();
}