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

  MenuItemAction flashItem = MenuItemAction("Flash", 2, [&]()
                                            {
                                              data_packet p;
                                              p.type = 0x30;
                                              p.len = 2;
                                              p.data[0] = 5;  // 5 flashes
                                              p.data[1] = 40 / 10; // 10; // 40ms
                                              wireless.send(&p, car_addr);
                                              //
                                            });

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
  menu.addMenuItem(&flashItem);
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