#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "Buttons.h"

class MenuScreen : public Screen
{
public:
  MenuScreen(String _name) : Screen(_name){};

  void draw() override;
  void update() override;

  int mainMenu = 0;
};

void MenuScreen::draw()
{

  switch (mainMenu)
  {
  case 0:
  case 1:
  {
    screenUtils.drawMenuItem("Settings", 10, 0, mainMenu == 0, TFT_RED);
    screenUtils.drawMenuItem("ESP NOW", 10, 1, mainMenu == 1, TFT_RED);
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
      screenManager.setScreen("Settings");
    break;

  case 1:
    if (ClickButtonDOWN.clicks == 2)
      screenManager.setScreen("Espnow");
    break;
  }

  mainMenu = constrain(mainMenu, 0, 1);
}