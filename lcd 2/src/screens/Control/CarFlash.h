#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Wireless.h"

class CarFlashScreen : public Screen
{
public:
  CarFlashScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  uint8_t count = 5;
  uint8_t delay = 50;
  // uint8_t count = preferences.getUChar("relay1FlashCount", 5);
  // uint8_t delay = preferences.getUChar("relay1FlashDelay", 50);

  MenuItemAction relay1FlashItem = MenuItemAction("Flash R 1", 2, [&]()
                                                  {
                                                    data_packet p;
                                                    p.type = CMD_RELAY_1_FLASH;
                                                    p.len = 2;
                                                    p.data[0] = count;
                                                    p.data[1] = uint8_t(delay / 10);

                                                    wireless.send(&p, car_addr);
                                                    //
                                                  });

  MenuItemNumber<uint8_t> relay1FlashCountItem = MenuItemNumber<uint8_t>("Count", &count, 1, 100);
  MenuItemNumber<uint8_t> relay1FlashDelayItem = MenuItemNumber<uint8_t>("Delay", &delay, 20, 250, 10);

  void draw() override;
  void update() override;
};

CarFlashScreen::CarFlashScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&relay1FlashItem);
  menu.addMenuItem(&relay1FlashCountItem);
  menu.addMenuItem(&relay1FlashDelayItem);

  // relay1FlashCountItem.setOnChange([this]()
  //                                  { preferences.putUChar("relay1FlashCount", count); });

  // relay1FlashDelayItem.setOnChange([this]()
  //                                  { preferences.putUChar("relay1FlashDelay", delay); });
}

void CarFlashScreen::draw()
{
  menu.draw();
}

void CarFlashScreen::update()
{
  menu.update();
}
