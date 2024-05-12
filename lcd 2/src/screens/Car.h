#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include "IO/Wireless.h"
#include "IO/Popup.h"

class CarScreen : public Screen
{
public:
  CarScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  // 80:65:99:4b:3a:d1

  MenuItemAction lockItem = MenuItemAction("Lock", 2, [&]()
                                           {
                                             packet p;
                                             p.type = 1;
                                             p.len = 1;
                                             p.data[0] = 0;

                                             int res = wireless.send(&p, car_addr);
                                             //
                                           });

  MenuItemAction unlockItem = MenuItemAction("Unlock", 2, [&]()
                                             {
                                               packet p;
                                               p.type = 1;
                                               p.len = 1;
                                               p.data[0] = 1;

                                               wireless.send(&p, car_addr);

                                               int res = wireless.send(&p, car_addr);
                                               //
                                             });

  void draw() override;
  void update() override;
};

CarScreen::CarScreen(String _name) : Screen(_name)
{

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&lockItem);
  menu.addMenuItem(&unlockItem);
}

void CarScreen::draw()
{
  // display.noTopBar();
  menu.draw();
}

void CarScreen::update()
{
  menu.update();
}