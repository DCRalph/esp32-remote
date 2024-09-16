#pragma once

#include "config.h"
#include "IO/Display.h"
#include <IO/Wireless.h>

class ControlScreen : public Screen
{
public:
  ControlScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate carLocksItem = MenuItemNavigate("Car Locks", "CarLocks");
  MenuItemNavigate carItem = MenuItemNavigate("Car relays", "Car");
  MenuItemAction flashItem = MenuItemAction("Flash", 2, [&]()
                                            {
                                              data_packet p;
                                              p.type = 0x30;
                                              p.len = 2;
                                              p.data[0] = 5;       // 5 flashes
                                              p.data[1] = 40 / 10; // 10; // 40ms
                                              wireless.send(&p, car_addr);
                                              //
                                            });

  void draw() override;
  void update() override;
};

ControlScreen::ControlScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&carLocksItem);
  menu.addMenuItem(&carItem);
  menu.addMenuItem(&flashItem);
}

void ControlScreen::draw()
{
  menu.draw();
}

void ControlScreen::update()
{
  menu.update();
}