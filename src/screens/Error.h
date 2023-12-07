#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "driver/Screen.h"

#include "buttons.h"

class ErrorScreen : public Screen
{
public:
  ErrorScreen(String _name, String _id) : Screen(_name, _id){};

  void draw() override;
  void update() override;
};

void ErrorScreen::draw()
{
  display.sprite.setTextDatum(MC_DATUM);

  display.sprite.setTextSize(5);
  display.sprite.setTextColor(TFT_RED);
  display.sprite.drawString("ERROR", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 50);

  display.sprite.setTextSize(2);
  display.sprite.setTextColor(TFT_WHITE);

  display.sprite.drawString("Press any button", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);
  display.sprite.drawString("to continue", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 60);
}

void ErrorScreen::update()
{
  if (ClickButton0.clicks != 0 || ClickButton21.clicks != 0)
    screenManager.setScreen("menu");
}