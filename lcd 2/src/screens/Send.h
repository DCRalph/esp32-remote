#pragma once

#include "config.h"
#include "Display.h"
#include "IO/Buttons.h"
#include "IO/Wireless.h"
#include "IO/Mesh.h"

auto syncMgr = SyncManager::getInstance();

class SendScreen : public Screen
{
public:
  SendScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  // 80:65:99:4b:3a:d1
  u8_t peer_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};

  uint16_t TRIGGER_EVENT_CHANNEL = 0x4111;
  uint16_t BEEP_EVENT_CHANNEL = 0x4112;
  EventHandle<uint8_t> triggerEvent;
  EventHandle<void> beepEvent;

  uint8_t lastTriggerValue = 0;
  uint32_t lastTriggerTime = 0;
  uint32_t lastTriggerFrom = 0;

  MenuItemAction sendItem = MenuItemAction("Send", 2, [&]()
                                           {
                                             TransportPacket p;
                                             p.type = 43;
                                             p.len = 5;

                                             memcpy(p.data, "Hello", 5);

                                             wireless.sendPacket(p, TransportAddress::fromMac(peer_addr));
                                             //
                                           });

  MenuItemAction sendTriggerItem = MenuItemAction("Trigger", 2, [&]()
                                                  {
                                                    triggerEvent.emit(lastTriggerValue);
                                                    lastTriggerValue++;
                                                    Serial.printf("Trigger sent: %d\n", lastTriggerValue);
                                                    //
                                                  });

  MenuItemAction sendMeshItem = MenuItemAction("Buzz", 2, [&]()
                                               {
                                                 beepEvent.emit();
                                                 Serial.println("Buzz sent");
                                                 //
                                               });

  void draw() override;
  void update() override;
  void onEnter() override;
  void onExit() override;
};

SendScreen::SendScreen(String _name) : Screen(_name)
{

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&sendItem);
  menu.addMenuItem(&sendTriggerItem);
  menu.addMenuItem(&sendMeshItem);
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

void SendScreen::onEnter()
{

  triggerEvent = syncMgr->event<uint8_t>(TRIGGER_EVENT_CHANNEL);
  beepEvent = syncMgr->event<void>(BEEP_EVENT_CHANNEL);

  triggerEvent.onEvent([this](uint32_t from, uint8_t v)
                       {
                         this->lastTriggerFrom = from;
                         this->lastTriggerValue = v;
                         this->lastTriggerTime = millis();
                         Serial.printf("Trigger received from %08X: %d\n", from, v);
                         //
                       });

  Serial.printf("Send opened registered\n");
}

void SendScreen::onExit()
{
  syncMgr->unregisterEvent(BEEP_EVENT_CHANNEL);
  beepEvent = EventHandle<void>();

  Serial.printf("Send closed unregistered\n");
}