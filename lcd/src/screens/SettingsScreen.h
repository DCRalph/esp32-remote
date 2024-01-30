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

  int settingsMenu = 0;
};

void Settings::draw()
{
  switch (settingsMenu)
  {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  {

    screenUtils.drawMenuItem("Back", 10, 0, settingsMenu == 0, TFT_RED);
    screenUtils.drawMenuItem("Battery", 10, 1, settingsMenu == 1, TFT_RED);
    screenUtils.drawMenuItem("Wi-Fi", 10, 2, settingsMenu == 2, TFT_RED);

    break;
  }
  }
}

void Settings::update()
{
  if (ClickButtonDOWN.clicks == 1)
    settingsMenu++;
  else if (ClickButtonUP.clicks == 1)
    settingsMenu--;

  switch (settingsMenu)
  {
  case 0:
    if (ClickButtonDOWN.clicks == 2)
      screenManager.setScreen("menu");
    break;
  case 1:
    if (ClickButtonDOWN.clicks == 2)
      screenManager.setScreen("battery");
    break;
  case 2:
    if (ClickButtonDOWN.clicks == 2)
      screenManager.setScreen("wifi");
    break;
  }

  settingsMenu = constrain(settingsMenu, 0, 2);
}