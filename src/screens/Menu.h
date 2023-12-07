#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "buttons.h"

class MenuScreen : public Screen
{
public:
  MenuScreen(String _name, String _id) : Screen(_name, _id){};

  void draw() override;
  void update() override;

  int mainMenu = 0;
};

void MenuScreen::draw()
{

  display.sprite.fillSmoothRoundRect(0, 40, LCD_WIDTH, 50, 20, TFT_WHITE);

  display.sprite.setTextSize(3);
  display.sprite.setTextColor(TFT_BLACK);
  display.sprite.setTextDatum(ML_DATUM);

  display.sprite.drawString("Main menu", 10, 65);

  switch (mainMenu)
  {
  case 0:
  case 1:
  case 2:
  {

    display.sprite.setTextSize(3);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.setTextDatum(TL_DATUM);

    screenUtils.drawMenuItem("RSSI", 10, 0, mainMenu == 0, TFT_RED);
    screenUtils.drawMenuItem("Desk Lamp", 10, 1, mainMenu == 1, TFT_GREEN);
    screenUtils.drawMenuItem("Menu 3", 10, 2, mainMenu == 2, TFT_BLUE);

    break;
  }
  case 3:
  case 4:
  case 5:
  {
    display.sprite.setTextSize(3);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.setTextDatum(TL_DATUM);

    screenUtils.drawMenuItem("Menu 4", 10, 0, mainMenu == 3, TFT_YELLOW);
    screenUtils.drawMenuItem("Menu 5", 10, 1, mainMenu == 4, TFT_ORANGE);
    screenUtils.drawMenuItem("Menu 6", 10, 2, mainMenu == 5, TFT_PURPLE);

    break;
  }
  }
}

void MenuScreen::update()
{

  if (ClickButton0.clicks == 1)
    mainMenu--;
  else if (ClickButton21.clicks == 1)
    mainMenu++;

  switch (mainMenu)
  {
  case 0:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("rssi");
    break;
  case 1:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("desk_lamp");
    break;
  case 2:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen(-1);
    break;
    /////////////////////////////
  case 3:
    break;
  case 4:
    break;
  case 5:
    break;
  }

  mainMenu = constrain(mainMenu, 0, 5);
}