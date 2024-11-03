#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include "IO/Wireless.h"

class RemoteRelayScreen : public Screen
{
public:
  RemoteRelayScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  // 80:65:99:4b:3a:d0
  u8_t peer_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};

  MenuItemAction testItem = MenuItemAction("Send", 2, [&]()
                                           {
                                             data_packet p;
                                             p.type = 43;
                                             p.len = 5;

                                             memcpy(p.data, "Hello", 5);

                                             wireless.send(&p, peer_addr);
                                             //
                                           });

  MenuItemAction relay1 = MenuItemAction("Relay 1", 2, [&]()
                                         {
                                           data_packet p;
                                           p.type = 11;
                                           p.len = 0;

                                           wireless.send(&p, peer_addr);
                                           //
                                         });

  MenuItemAction relay2 = MenuItemAction("Relay 2", 2, [&]()
                                         {
                                           data_packet p;
                                           p.type = 12;
                                           p.len = 0;

                                           wireless.send(&p, peer_addr);
                                           //
                                         });

  void draw() override;
  void update() override;
  void onExit() override;
};

RemoteRelayScreen::RemoteRelayScreen(String _name) : Screen(_name)
{

  relay1.setTextColor(TFT_RED);
  relay2.setTextColor(TFT_BLUE);

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&testItem);
  menu.addMenuItem(&relay1);
  menu.addMenuItem(&relay2);
}

void RemoteRelayScreen::draw()
{
  // display.noTopBar();
  menu.draw();
}

void RemoteRelayScreen::update()
{
  menu.update();
  if (menu.items[menu.getActive()] == &relay1)
  {
    btnLed.SetColor(255, 0, 0);
    if (ClickButtonTRIGGER.clicks == 1)
      relay1.executeFunc(2);
  }

  if (menu.items[menu.getActive()] == &relay2)
  {
    btnLed.SetColor(0, 0, 255);
    if (ClickButtonTRIGGER.clicks == 1)
      relay2.executeFunc(2);
  }

  if (menu.items[menu.getActive()] != &relay1 && menu.items[menu.getActive()] != &relay2)
  {
    btnLed.Off();
  }
}

void RemoteRelayScreen::onExit()
{
  btnLed.Off();
}