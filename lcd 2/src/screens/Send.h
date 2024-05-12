#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include "IO/Wireless.h"

class SendScreen : public Screen
{
public:
  SendScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  // 80:65:99:4b:3a:d1
  u8_t peer_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};

  MenuItemAction sendItem = MenuItemAction("Send", 2, [&]()
                                           {
                                             packet p;
                                             p.type = 43;
                                             p.len = 5;

                                             memcpy(p.data, "Hello", 5);

                                             wireless.send(&p, peer_addr);
                                             //
                                           });

  void draw() override;
  void update() override;
};

SendScreen::SendScreen(String _name) : Screen(_name)
{

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&sendItem);
}

void SendScreen::draw()
{
  // display.noTopBar();
  menu.draw();
}

void SendScreen::update()
{
  menu.update();
}