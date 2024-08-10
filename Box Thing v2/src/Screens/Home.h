#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Screens/Helpers/Menu.h"

class HomeScreen : public Screen
{
public:
  HomeScreen(String _name);

  Menu menu = Menu();

  MenuItemNavigate switchMenuItem = MenuItemNavigate("Control", "Switch Menu");

  MenuItemNavigate settingsItem = MenuItemNavigate("Settings", "Settings");

  MenuItemAction powerOffItem = MenuItemAction(
      "Power Off", -1, []()
      { screenManager.setScreen("Shutdown"); });

  void draw() override;
  void update() override;
  void onEnter() override;
  uint8_t active;
};

HomeScreen::HomeScreen(String _name) : Screen(_name)
{
  active = 1;

  menu.addMenuItem(&switchMenuItem);
  menu.addMenuItem(&settingsItem);
  menu.addMenuItem(&powerOffItem);

  settingsItem.addRoute(2, "Debug");
}

void HomeScreen::draw()
{
  menu.draw();
}

void HomeScreen::update()
{
  menu.update();
}

void HomeScreen::onEnter()
{
}