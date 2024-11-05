#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
// #include "IO/myespnow.h"
#include "IO/Wireless.h"

class RemoteRelayScreen : public Screen
{
public:
  RemoteRelayScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  bool connectionPing = false;
  uint64_t lastPing = 0;

  u8_t peer_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};

  MenuItemToggle connectionItem = MenuItemToggle("Conn", &connectionPing, false);

  MenuItemAction testItem = MenuItemAction("Send", 1, [&]()
                                           {
                                             data_packet p;
                                             p.type = 43;
                                             p.len = 5;

                                             memcpy(p.data, "Hello", 5);

                                             wireless.send(&p, peer_addr);
                                             //
                                           });

    MenuItemAction relay1 = MenuItemAction("Relay 1", 1, [&]()
                                         {
                                           data_packet p;
                                           p.type = 11;
                                           p.len = 0;

                                           wireless.send(&p, peer_addr);
                                           //
                                         });

  MenuItemAction relay2 = MenuItemAction("Relay 2", 1, [&]()
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

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&connectionItem);
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

  if (millis() - lastPing > 100)
  {
    lastPing = millis();

    connectionPing = wireless.lastStatus == ESP_OK;

    data_packet p;
    p.type = 0;
    p.len = 0;

    wireless.send(&p, peer_addr);
  }
}

void RemoteRelayScreen::onExit()
{
}