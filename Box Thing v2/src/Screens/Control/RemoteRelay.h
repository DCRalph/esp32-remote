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

static void fireRelay(int relay)
{
  bool relays[8] = {false, false, false, false, false, false, false, false};

  relays[relay] = true;

  fullPacket fp;
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, remote_addr, 6);
  fp.p.type = CMD_FIRE;
  fp.p.len = 8;

  memcpy(fp.p.data, relays, 8);

  wireless.send(&fp);
}

class RemoteRelayScreen : public Screen
{
public:
  RemoteRelayScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  uint64_t lastPing = 0;
  uint64_t lastConfirmedPing = 0;
  bool connected = false;
  bool lastConnected = false;

  bool armed = false;

  MenuItemToggle connectionItem = MenuItemToggle("Conn", &connected, false);

  MenuItemAction testItem = MenuItemAction("Test", 1, [&]()
                                           {
                                             fullPacket fp;
                                             fp.direction = PacketDirection::SEND;
                                             memcpy(fp.mac, remote_addr, 6);
                                             fp.p.type = CMD_TEST;
                                             fp.p.len = 0;

                                             wireless.send(&fp); });

  MenuItemToggle armItem = MenuItemToggle("Armed", &armed);

  MenuItemAction fire1Item = MenuItemAction("Fire 1", 1, [&]()
                                            {
                                              if (connected && armed)
                                                fireRelay(0);
                                              //
                                            });

  MenuItemAction fire2Item = MenuItemAction("Fire 2", 1, [&]()
                                            {
                                              if (connected && armed)
                                                fireRelay(1);
                                              //
                                            });

  MenuItemAction fire3Item = MenuItemAction("Fire 3", 1, [&]()
                                            {
                                              if (connected && armed)
                                                fireRelay(2);
                                              //
                                            });

  MenuItemAction fire4Item = MenuItemAction("Fire 4", 1, [&]()
                                            {
                                              if (connected && armed)
                                                fireRelay(3);
                                              //
                                            });

  MenuItemAction fire5Item = MenuItemAction("Fire 5", 1, [&]()
                                            {
                                              if (connected && armed)
                                                fireRelay(4);
                                              //
                                            });

  MenuItemAction fire6Item = MenuItemAction("Fire 6", 1, [&]()
                                            {
                                              if (connected && armed)
                                                fireRelay(5);
                                              //
                                            });

  MenuItemAction fire7Item = MenuItemAction("Fire 7", 1, [&]()
                                            {
                                              if (connected && armed)
                                                fireRelay(6);
                                              //
                                            });

  MenuItemAction fire8Item = MenuItemAction("Fire 8", 1, [&]()
                                            {
                                              if (connected && armed)
                                                fireRelay(7);
                                              //
                                            });

  void draw() override;
  void update() override;
  void onEnter() override;
};

RemoteRelayScreen::RemoteRelayScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);

  menu.addMenuItem(&connectionItem);
  menu.addMenuItem(&armItem);
  menu.addMenuItem(&testItem);

  menu.addMenuItem(&fire1Item);
  menu.addMenuItem(&fire2Item);
  menu.addMenuItem(&fire3Item);
  menu.addMenuItem(&fire4Item);
  menu.addMenuItem(&fire5Item);
  menu.addMenuItem(&fire6Item);
  menu.addMenuItem(&fire7Item);
  menu.addMenuItem(&fire8Item);

  armItem.setOnChange([&]()
                      {
                        fullPacket fp;
                        fp.direction = PacketDirection::SEND;
                        memcpy(fp.mac, remote_addr, 6);
                        fp.p.len = 0;

                        if (armed)
                        {
                          fp.p.type = CMD_ARM;
                        }
                        else
                        {
                          fp.p.type = CMD_DISARM;
                        }

                        wireless.send(&fp);
                        //
                      });
}

void RemoteRelayScreen::draw()
{
  menu.draw();
}

void RemoteRelayScreen::update()
{
  menu.update();

  connected = millis() - lastConfirmedPing < 1000;

  if (connected != lastConnected)
  {
    lastConnected = connected;

    if (!connected)
    {
      armed = false;
    }
  }

  if (millis() - lastPing > 200)
  {
    lastPing = millis();

    fullPacket fp;
    fp.direction = PacketDirection::SEND;
    memcpy(fp.mac, remote_addr, 6);
    fp.p.type = CMD_PING;
    fp.p.len = 0;

    wireless.send(&fp);
  }
}

void RemoteRelayScreen::onEnter()
{
}