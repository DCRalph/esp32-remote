#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "Buttons.h"

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

  display.sprite.fillSmoothRoundRect(0, 25, LCD_WIDTH, 30, 20, TFT_WHITE);

  display.sprite.setTextSize(2);
  display.sprite.setTextColor(TFT_BLACK);
  display.sprite.setTextDatum(ML_DATUM);

  display.sprite.drawString("Main menu", 10, 40);

  switch (mainMenu)
  {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  {

    screenUtils.drawMenuItem("RSSI", 10, 0, mainMenu == 0, TFT_RED);
    screenUtils.drawMenuItem("Settings", 10, 1, mainMenu == 1, TFT_RED);
    break;
  }
  }
}

void MenuScreen::update()
{
  if (ClickButtonDOWN.clicks == 1)
    mainMenu++;
  else if (ClickButtonUP.clicks == 1)
    mainMenu--;

  switch (mainMenu)
  {
  case 0:
    if (ClickButtonDOWN.clicks == 2)
      screenManager.setScreen("rssi");
    break;
  case 1:
    if (ClickButtonDOWN.clicks == 2)
      screenManager.setScreen("settings");
    break;

  }

  mainMenu = constrain(mainMenu, 0, 4);
}