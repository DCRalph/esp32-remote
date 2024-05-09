#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"

class MenuScreen : public Screen
{
public:
  MenuScreen(String _name);

  Menu menu = Menu();

  MenuItemNavigate settingsMenuItem = MenuItemNavigate("Settings", "Settings");

  long testValue = 0;
  bool testBool = false;

  MenuItemNumber testItem = MenuItemNumber("test", &testValue, -10, 10);

  MenuItemToggle testItem1 = MenuItemToggle("test1", &testBool);

  void draw() override;
  void update() override;
};

MenuScreen::MenuScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&settingsMenuItem);
  menu.addMenuItem(&testItem);
  menu.addMenuItem(&testItem1);
}

void MenuScreen::draw()
{
  // display.noTopBar();
  menu.draw();
}

void MenuScreen::update()
{
  menu.update();
}