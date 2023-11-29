#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "driver/Screen.h"

#include "buttons.h"

class ErrorScreen : public Screen
{
public:
  ErrorScreen(TFT_eSprite *_sprite, String _name) : Screen(_sprite, _name){};

  void draw() override;
  void update() override;
};

void ErrorScreen::draw()
{
  sprite->fillSprite(TFT_BLACK);
  drawTopBar(sprite, name);

  sprite->setTextDatum(MC_DATUM);

  sprite->setTextSize(5);
  sprite->setTextColor(TFT_RED);
  sprite->drawString("ERROR", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 50);

  sprite->setTextSize(2);
  sprite->setTextColor(TFT_WHITE);

  sprite->drawString("Press any button", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);
  sprite->drawString("to continue", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 60);
}

void ErrorScreen::update()
{
  if (ClickButton0.clicks != 0 || ClickButton21.clicks != 0)
    currentScreen = 0;
}