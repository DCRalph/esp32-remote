#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"

class HomeScreen : public Screen
{
public:
  HomeScreen(String _name);

  Menu menu = Menu();

  MenuItemNavigate ControlMenuItem = MenuItemNavigate("Control", "Control");
  MenuItemNavigate settingsMenuItem = MenuItemNavigate("Settings", "Settings");
  MenuItemNavigate sendMenuItem = MenuItemNavigate("Send", "Send");

  MenuItemAction testPopupItem = MenuItemAction("Test Popup", 2, []()
                                                {
                                                  screenManager.showPopup(new Popup("Test Popup", "This is a test gyY9 popup. pls wrap words TTTTT"));
                                                  //
                                                });

                                                  MenuItemAction testPopupItem2 = MenuItemAction("Test Popup 2", 2, []()
                                                {
                                                  screenManager.showPopup(new AutoClosePopup("Test Popup 2", "This is a test", 5000));
                                                  //
                                                });

  void draw() override;
  void update() override;
};

HomeScreen::HomeScreen(String _name) : Screen(_name)
{

  settingsMenuItem.setTextColor(TFT_RED);
  settingsMenuItem.setBgColor(TFT_RED);

  menu.addMenuItem(&ControlMenuItem);
  ControlMenuItem.addRoute(3, "CarLocks"); 


  menu.addMenuItem(&settingsMenuItem);
  menu.addMenuItem(&sendMenuItem);
  menu.addMenuItem(&testPopupItem);
  menu.addMenuItem(&testPopupItem2);
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