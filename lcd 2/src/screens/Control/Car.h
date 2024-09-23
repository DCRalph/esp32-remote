#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Wireless.h"

static void relayChangeCb(uint8_t cmd, bool *global)
{
  fullPacket fp;
  memcpy(fp.mac, car_addr, 6);
  fp.direction = PacketDirection::SEND;
  fp.p.type = cmd;
  fp.p.data[0] = !*global;

  wireless.send(&fp);
}

class CarControlScreen : public Screen
{
public:
  CarControlScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemAction lockDoorItem = MenuItemAction("Lock Door", 2, []()
                                               {
                                                 data_packet p;
                                                 p.type = CMD_DOOR_LOCK;
                                                 p.len = 1;
                                                 p.data[0] = 0;

                                                 wireless.send(&p, car_addr);
                                                 //
                                               });

  MenuItemAction unlockDoorItem = MenuItemAction("Unlock Door", 2, []()
                                                 {
                                                   data_packet p;
                                                   p.type = CMD_DOOR_LOCK;
                                                   p.len = 1;
                                                   p.data[0] = 1;

                                                   wireless.send(&p, car_addr);
                                                   //
                                                 });

  MenuItemNavigate carFlashItem = MenuItemNavigate("Flash", "CarFlash");

  MenuItemToggle relay1Item = MenuItemToggle("R 1", &globalRelay1, false);
  MenuItemToggle relay2Item = MenuItemToggle("R 2", &globalRelay2, false);
  MenuItemToggle relay3Item = MenuItemToggle("R 3", &globalRelay3, false);
  MenuItemToggle relay4Item = MenuItemToggle("R 4", &globalRelay4, false);
  MenuItemToggle relay5Item = MenuItemToggle("R 5", &globalRelay5, false);
  MenuItemToggle relay6Item = MenuItemToggle("R 6", &globalRelay6, false);

  void draw() override;
  void update() override;
  void onEnter() override;
};

CarControlScreen::CarControlScreen(String _name) : Screen(_name)
{
  topBarColor = TFT_WHITE;
  topBarTextColor = TFT_BLACK;

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&lockDoorItem);
  menu.addMenuItem(&unlockDoorItem);
  menu.addMenuItem(&carFlashItem);
  menu.addMenuItem(&relay1Item);
  menu.addMenuItem(&relay2Item);
  menu.addMenuItem(&relay3Item);
  menu.addMenuItem(&relay4Item);
  menu.addMenuItem(&relay5Item);
  menu.addMenuItem(&relay6Item);

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
}

void CarControlScreen::draw()
{
  menu.draw();
}

void CarControlScreen::update()
{
  menu.update();
}

void CarControlScreen::onEnter()
{
  fullPacket fp;
  memcpy(fp.mac, car_addr, 6);
  fp.direction = PacketDirection::SEND;
  fp.p.type = CMD_RELAY_ALL;
  fp.p.len = 0;

  wireless.send(&fp);
}
