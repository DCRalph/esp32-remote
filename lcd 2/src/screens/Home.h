#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"

class HomeScreen : public Screen
{
public:
  HomeScreen(String _name);

  Menu menu = Menu();

  MenuItemNavigate settingsMenuItem = MenuItemNavigate("Settings", "Settings");
  MenuItemNavigate sendMenuItem = MenuItemNavigate("Send", "Send");
  MenuItemNavigate carMenuItem = MenuItemNavigate("Car", "Car");

  void draw() override;
  void update() override;
};

HomeScreen::HomeScreen(String _name) : Screen(_name)
{

  settingsMenuItem.setTextColor(TFT_RED);
  settingsMenuItem.setBgColor(TFT_RED);

  menu.addMenuItem(&settingsMenuItem);
  menu.addMenuItem(&sendMenuItem);
  menu.addMenuItem(&carMenuItem);
}

void HomeScreen::draw()
{
  // display.noTopBar();
  menu.draw();
}

void HomeScreen::update()
{
  menu.update();
}