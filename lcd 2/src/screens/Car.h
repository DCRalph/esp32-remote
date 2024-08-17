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

  // Menu menu = Menu();

  // MenuItemBack backItem;

  // 80:65:99:4b:3a:d1

  // MenuItemAction lockItem = MenuItemAction("Lock", 2, [&]()
  //                                          {
  //                                            packet p;
  //                                            p.type = 11; // car locks
  //                                            p.len = 1;
  //                                            p.data[0] = 0; // lock

  //                                            wireless.send(&p, car_addr);
  //                                            //
  //                                          });

  // MenuItemAction unlockItem = MenuItemAction("Unlock", 2, [&]()
  //                                            {
  //                                              packet p;
  //                                              p.type = 11; // car locks
  //                                              p.len = 1;
  //                                              p.data[0] = 1; // unlock

  //                                              wireless.send(&p, car_addr);
  //                                              //
  //                                            });

  void draw() override;
  void update() override;
};

CarScreen::CarScreen(String _name) : Screen(_name)
{

  // menu.addMenuItem(&backItem);
  // menu.addMenuItem(&lockItem);
  // menu.addMenuItem(&unlockItem);
}

void CarScreen::draw()
{
  // display.noTopBar();
  // menu.draw();

  display.sprite.setTextDatum(TR_DATUM);
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.setTextSize(3);
  display.sprite.drawString("Lock ->", LCD_WIDTH - 2, 25);

  display.sprite.setTextDatum(BR_DATUM);
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.setTextSize(3);
  display.sprite.drawString("Unlock ->", LCD_WIDTH - 2, LCD_HEIGHT);
}

void CarScreen::update()
{
  // menu.update();

  if (ClickButtonUP.clicks == 1)
  {
    data_packet p;
    p.type = 11; // car locks
    p.len = 1;
    p.data[0] = 0; // lock

    wireless.send(&p, car_addr);
  }

  if (ClickButtonDOWN.clicks == 1)
  {
    data_packet p;
    p.type = 11; // car locks
    p.len = 1;
    p.data[0] = 1; // unlock

    wireless.send(&p, car_addr);
  }

  if (ClickButtonUP.clicks == 2)
    screenManager.back();
}