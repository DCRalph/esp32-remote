#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "driver/Screen.h"

#include "buttons.h"

class MenuScreen : public Screen
{
public:
  MenuScreen(TFT_eSprite *_sprite, String _name) : Screen(_sprite, _name){};

  void draw() override;
  void update() override;

  int mainMenu = 0;
};

void MenuScreen::draw()
{
  sprite->fillSprite(TFT_BLACK);
  drawTopBar(sprite, name);

  sprite->fillSmoothRoundRect(0, 40, LCD_WIDTH, 50, 20, TFT_WHITE);

  sprite->setTextSize(3);
  sprite->setTextColor(TFT_BLACK);
  sprite->setTextDatum(ML_DATUM);

  sprite->drawString("Main menu", 10, 65);

  switch (mainMenu)
  {
  case 0:
  case 1:
  case 2:
  {

    sprite->setTextSize(3);
    sprite->setTextColor(TFT_WHITE);
    sprite->setTextDatum(TL_DATUM);

    // if (mainMenu == 0)
    //   sprite->setTextColor(TFT_BLUE);
    // else
    //   sprite->setTextColor(TFT_WHITE);
    // sprite->drawString("Menu 1", 10, 120);
    // sprite->setTextColor(TFT_WHITE);

    drawMenuItem(sprite, "RSSI", 10, 120, mainMenu == 0, TFT_RED);
    drawMenuItem(sprite, "Menu 2", 10, 155, mainMenu == 1, TFT_GREEN);
    drawMenuItem(sprite, "Menu 3", 10, 190, mainMenu == 2, TFT_BLUE);

    break;
  }
  case 3:
  case 4:
  case 5:
  {
    sprite->setTextSize(3);
    sprite->setTextColor(TFT_WHITE);
    sprite->setTextDatum(TL_DATUM);

    if (mainMenu == 3)
      sprite->setTextColor(TFT_BLUE);
    else
      sprite->setTextColor(TFT_WHITE);
    sprite->drawString("Menu 4", 10, 120);

    if (mainMenu == 4)
      sprite->setTextColor(TFT_BLUE);
    else
      sprite->setTextColor(TFT_WHITE);
    sprite->drawString("Menu 5", 10, 155);

    if (mainMenu == 5)
      sprite->setTextColor(TFT_BLUE);
    else
      sprite->setTextColor(TFT_WHITE);
    sprite->drawString("Menu 6", 10, 190);

    break;
  }
  }
}

void MenuScreen::update()
{

  switch (mainMenu)
  {
  case 0:
  case 1:
  case 2:
  {

    if (ClickButton0.clicks == -1 && mainMenu == 0)
      currentScreen = 1;

    if (ClickButton0.clicks == -1 && mainMenu == 1)
      currentScreen = -1;

    if (ClickButton0.clicks == 1)
      mainMenu--;
    else if (ClickButton21.clicks == 1)
      mainMenu++;

    break;
  }
  case 3:
  case 4:
  case 5:
  {

    if (ClickButton0.clicks == 1)
      mainMenu--;
    else if (ClickButton21.clicks == 1)
      mainMenu++;

    break;
  }
  }

  mainMenu = constrain(mainMenu, 0, 5);
}