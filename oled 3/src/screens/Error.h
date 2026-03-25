#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"

class ErrorScreen : public Screen
{
public:
  ErrorScreen(String _name) : Screen(_name){};

  void draw() override;
  void update() override;
};

void ErrorScreen::draw()
{
  display.setTextDatum(TC_DATUM);

  display.setTextSize(5);
  display.setTextColor(TFT_RED);
  display.drawString("ERROR", LCD_WIDTH / 2, 50);

  display.setTextSize(2);
  display.setTextColor(TFT_WHITE);

  display.drawString("Press any button", LCD_WIDTH / 2, 100);
  display.drawString("to continue", LCD_WIDTH / 2, 130);
}

void ErrorScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.setScreen("Home");
}