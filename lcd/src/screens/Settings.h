#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "Buttons.h"

class Settings : public Screen
{
public:
  Settings(String _name, String _id) : Screen(_name, _id){};

  void draw() override;
  void update() override;
};

void Settings::draw()
{
  display.sprite.fillSmoothRoundRect(0, 25, LCD_WIDTH, 30, 20, TFT_WHITE);
}

void Settings::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.setScreen("menu");
}