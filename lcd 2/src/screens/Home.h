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

  MenuItemAction testPopupItem = MenuItemAction("Test Popup", 2, []()
                                                { screenManager.showPopup(new Popup("Test Popup", "This is a test gyY9 popup. pls wrap words TTTTT")); });

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
  menu.addMenuItem(&testPopupItem);
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