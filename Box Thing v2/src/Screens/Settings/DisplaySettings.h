#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Screens/Helpers/Menu.h"

class DisplaySettingsScreen : public Screen
{
public:
  DisplaySettingsScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  void draw() override;
  void update() override;
};

DisplaySettingsScreen::DisplaySettingsScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
}

void DisplaySettingsScreen::draw()
{
  menu.draw();
}

void DisplaySettingsScreen::update()
{
  menu.update();
}