#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Wireless.h"
#include "IO/GPIO.h"

class ControlScreen : public Screen
{
public:
  ControlScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate carLocksItem = MenuItemNavigate("Car Locks", "CarLocks");
  MenuItemNavigate carItem = MenuItemNavigate("Car Control", "Car");

  MenuItemAction flashItemPreset1 = MenuItemAction("Flash 5 30", 2, [&]()
                                                   {
                                                     data_packet p;
                                                     p.type = 0x30;
                                                     p.len = 2;
                                                     p.data[0] = 5;       // 5 flashes
                                                     p.data[1] = 30 / 10; // 10; // 40ms
                                                     wireless.send(&p, car_addr);
                                                     //
                                                   });

  MenuItemAction flashItemPreset2 = MenuItemAction("Flash 2 40", 2, [&]()
                                                   {
                                                     data_packet p;
                                                     p.type = 0x30;
                                                     p.len = 2;
                                                     p.data[0] = 2;       // 3 flashes
                                                     p.data[1] = 40 / 10; // 10; // 40ms
                                                     wireless.send(&p, car_addr);
                                                     //
                                                   });

  MenuItemAction flashItemPreset3 = MenuItemAction("Flash 8 50", 2, [&]()
                                                   {
                                                     data_packet p;
                                                     p.type = 0x30;
                                                     p.len = 2;
                                                     p.data[0] = 8;       // 3 flashes
                                                     p.data[1] = 50 / 10; // 10; // 40ms
                                                     wireless.send(&p, car_addr);
                                                     //
                                                   });

  void draw() override;
  void update() override;
  void onEnter() override;
  void onExit() override;
};

ControlScreen::ControlScreen(String _name) : Screen(_name)
{
  topBarColor = TFT_CYAN;
  topBarTextColor = TFT_BLACK;

  menu.addMenuItem(&backItem);
  // menu.addMenuItem(&carLocksItem);
  menu.addMenuItem(&carItem);
  menu.addMenuItem(&flashItemPreset1);
  menu.addMenuItem(&flashItemPreset2);
  menu.addMenuItem(&flashItemPreset3);
}

void ControlScreen::draw()
{
  menu.draw();
}

void ControlScreen::update()
{
  menu.update();
}

void ControlScreen::onEnter()
{
  btnLed.SetColor565(topBarColor);
}

void ControlScreen::onExit()
{
  btnLed.Off();
}
