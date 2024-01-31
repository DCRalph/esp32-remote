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

  switch (mainMenu)
  {
  case 0:
  {

    screenUtils.drawMenuItem("Settings", 10, 0, mainMenu == 0, TFT_RED);
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
      screenManager.setScreen("settings");
    break;
  }

  mainMenu = constrain(mainMenu, 0, 0);
}