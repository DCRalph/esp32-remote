// #pragma once

// #include "config.h"
// #include "IO/Display.h"
// #include "IO/GPIO.h"
// // #include "IO/myespnow.h"
// #include "IO/Wireless.h"

// class RemoteRelayScreen : public Screen
// {
// public:
//   RemoteRelayScreen(String _name);

//   Menu menu = Menu();

//   MenuItemBack backItem;

//   bool connectionPing = false;
//   uint64_t lastPing = 0;

//   u8_t peer_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};

//   MenuItemToggle connectionItem = MenuItemToggle("Conn", &connectionPing, false);

//   MenuItemAction testItem = MenuItemAction("Send", 1, [&]()
//                                            {
//                                              data_packet p;
//                                              p.type = 43;
//                                              p.len = 5;

//                                              memcpy(p.data, "Hello", 5);

//                                              wireless.send(&p, peer_addr);
//                                              //
//                                            });

//     MenuItemAction relay1 = MenuItemAction("Relay 1", 1, [&]()
//                                          {
//                                            data_packet p;
//                                            p.type = 11;
//                                            p.len = 0;

//                                            wireless.send(&p, peer_addr);
//                                            //
//                                          });

//   MenuItemAction relay2 = MenuItemAction("Relay 2", 1, [&]()
//                                          {
//                                            data_packet p;
//                                            p.type = 12;
//                                            p.len = 0;

//                                            wireless.send(&p, peer_addr);
//                                            //
//                                          });

//   void draw() override;
//   void update() override;
//   void onExit() override;
// };

// RemoteRelayScreen::RemoteRelayScreen(String _name) : Screen(_name)
// {

//   menu.addMenuItem(&backItem);
//   menu.addMenuItem(&connectionItem);
//   menu.addMenuItem(&testItem);
//   menu.addMenuItem(&relay1);
//   menu.addMenuItem(&relay2);
// }

// void RemoteRelayScreen::draw()
// {
//   // display.noTopBar();
//   menu.draw();
// }

// void RemoteRelayScreen::update()
// {
//   menu.update();

//   if (millis() - lastPing > 100)
//   {
//     lastPing = millis();

//     connectionPing = wireless.lastStatus == ESP_OK;

//     data_packet p;
//     p.type = 0xa0;
//     p.len = 0;

//     wireless.send(&p, peer_addr);
//   }
// }

// void RemoteRelayScreen::onExit()
// {
// }

#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
// #include "IO/myespnow.h"
#include "IO/Wireless.h"

static void relayChangeCb(uint8_t cmd, bool *global)
{
  fullPacket fp;
  memcpy(fp.mac, remote_addr, 6);
  fp.direction = PacketDirection::SEND;
  fp.p.type = cmd;
  fp.p.len = 1;
  fp.p.data[0] = !*global;

  wireless.send(&fp);
}

class RemoteRelayScreen : public Screen
{
public:
  RemoteRelayScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;
  MenuItemToggle relay1Item = MenuItemToggle("R 1", &globalRelay1, false);
  MenuItemToggle relay2Item = MenuItemToggle("R 2", &globalRelay2, false);
  MenuItemToggle relay3Item = MenuItemToggle("R 3", &globalRelay3, false);
  MenuItemToggle relay4Item = MenuItemToggle("R 4", &globalRelay4, false);
  MenuItemToggle relay5Item = MenuItemToggle("R 5", &globalRelay5, false);
  MenuItemToggle relay6Item = MenuItemToggle("R 6", &globalRelay6, false);
  MenuItemToggle relay7Item = MenuItemToggle("R 7", &globalRelay7, false);
  MenuItemToggle relay8Item = MenuItemToggle("R 8", &globalRelay8, false);

  uint64_t lastFetch = 0;

  void draw() override;
  void update() override;
  void onEnter() override;
};

RemoteRelayScreen::RemoteRelayScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);

  menu.addMenuItem(&relay1Item);
  menu.addMenuItem(&relay2Item);
  menu.addMenuItem(&relay3Item);
  menu.addMenuItem(&relay4Item);
  menu.addMenuItem(&relay5Item);
  menu.addMenuItem(&relay6Item);
  menu.addMenuItem(&relay7Item);
  menu.addMenuItem(&relay8Item);

  relay1Item.setOnChange([]()
                         { relayChangeCb(CMD_RELAY_1_SET, &globalRelay1); });

  relay2Item.setOnChange([]()
                         { relayChangeCb(CMD_RELAY_2_SET, &globalRelay2); });

  relay3Item.setOnChange([]()
                         { relayChangeCb(CMD_RELAY_3_SET, &globalRelay3); });

  relay4Item.setOnChange([]()
                         { relayChangeCb(CMD_RELAY_4_SET, &globalRelay4); });

  relay5Item.setOnChange([]()
                         { relayChangeCb(CMD_RELAY_5_SET, &globalRelay5); });

  relay6Item.setOnChange([]()
                         { relayChangeCb(CMD_RELAY_6_SET, &globalRelay6); });

  relay7Item.setOnChange([]()
                         { relayChangeCb(CMD_RELAY_7_SET, &globalRelay7); });

  relay8Item.setOnChange([]()
                         { relayChangeCb(CMD_RELAY_8_SET, &globalRelay8); });
}

void RemoteRelayScreen::draw()
{
  menu.draw();
}

void RemoteRelayScreen::update()
{
  menu.update();

  if (millis() - lastFetch > 1000)
  {
    lastFetch = millis();

    fullPacket fp;
    memcpy(fp.mac, remote_addr, 6);
    fp.direction = PacketDirection::SEND;
    fp.p.type = CMD_RELAY_ALL;
    fp.p.len = 0;

    wireless.send(&fp);
  }
}

void RemoteRelayScreen::onEnter()
{

  fullPacket fp;
  memcpy(fp.mac, remote_addr, 6);
  fp.direction = PacketDirection::SEND;
  fp.p.type = CMD_RELAY_ALL;
  fp.p.len = 0;

  wireless.send(&fp);
}