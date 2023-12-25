#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "Buttons.h"

class ErrorScreen : public Screen
{
public:
  ErrorScreen(String _name, String _id) : Screen(_name, _id){};

  void draw() override;
  void update() override;
};

void ErrorScreen::draw()
{
  display.sprite.setTextDatum(TC_DATUM);

  display.sprite.setTextSize(5);
  display.sprite.setTextColor(TFT_RED);
  display.sprite.drawString("ERROR", LCD_WIDTH / 2, 50);

  display.sprite.setTextSize(2);
  display.sprite.setTextColor(TFT_WHITE);

  display.sprite.drawString("Press any button", LCD_WIDTH / 2, 100);
  display.sprite.drawString("to continue", LCD_WIDTH / 2, 130);
}

void ErrorScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.setScreen("menu");
}