#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "Buttons.h"

class Settings : public Screen
{
public:
  Settings(String _name) : Screen(_name){};

  void draw() override;
  void update() override;

  int settingsMenu = 0;
};

void Settings::draw()
{
  switch (settingsMenu)
  {
  case 0 ... 3:
  {

    screenUtils.drawMenuItem("Back", 10, 0, settingsMenu == 0, TFT_RED);
    screenUtils.drawMenuItem("Battery", 10, 1, settingsMenu == 1, TFT_RED);
    screenUtils.drawMenuItem("Wi-Fi", 10, 2, settingsMenu == 2, TFT_RED);
    screenUtils.drawMenuItem("RSSI", 10, 3, settingsMenu == 3, TFT_RED);

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
      screenManager.setScreen("Menu");
    break;
  case 1:
    if (ClickButtonDOWN.clicks == 2)
      screenManager.setScreen("Battery");
    break;
  case 2:
    if (ClickButtonDOWN.clicks == 2)
      screenManager.setScreen("Wi-Fi");
    break;
  case 3:
    if (ClickButtonDOWN.clicks == 2)
      screenManager.setScreen("RSSI");
    break;
  }

  settingsMenu = constrain(settingsMenu, 0, 3);
}