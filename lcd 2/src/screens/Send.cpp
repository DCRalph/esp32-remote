#include "Send.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"
#include "Wireless.h"
#include "Mesh.h"

namespace SendScreenNamespace
{

  static SyncManager *syncMgr = SyncManager::getInstance();

  namespace
  {
    static Menu menu;
    static MenuItemBack backItem;

    static u8_t peer_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};

    static constexpr uint16_t TRIGGER_EVENT_CHANNEL = 0x4111;
    static constexpr uint16_t BEEP_EVENT_CHANNEL = 0x4112;
    static EventHandle<uint8_t> triggerEvent;
    static EventHandle<void> beepEvent;

    static uint8_t lastTriggerValue = 0;
    static uint32_t lastTriggerTime = 0;
    static uint32_t lastTriggerFrom = 0;

    static MenuItemAction sendItem{"Send", 2, [&]()
                                   {
                                     TransportPacket p;
                                     p.type = 43;
                                     p.len = 5;

                                     memcpy(p.data, "Hello", 5);

                                     wireless.sendPacket(p, TransportAddress::fromMac(peer_addr));
                                   }};

    static MenuItemAction sendTriggerItem{"Trigger", 2, [&]()
                                          {
                                            triggerEvent.emit(lastTriggerValue);
                                            lastTriggerValue++;
                                            Serial.printf("Trigger sent: %d\n", lastTriggerValue);
                                          }};

    static MenuItemAction sendMeshItem{"Buzz", 2, [&]()
                                       {
                                         beepEvent.emit();
                                         Serial.println("Buzz sent");
                                       }};

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      menu.addMenuItem(&backItem);
      menu.addMenuItem(&sendItem);
      menu.addMenuItem(&sendTriggerItem);
      menu.addMenuItem(&sendMeshItem);
    }
  }

  void draw()
  {
    ensureInitialized();
    menu.draw();
  }

  void update()
  {
    ensureInitialized();
    menu.update();
  }

  void onEnter()
  {
    ensureInitialized();
    triggerEvent = syncMgr->event<uint8_t>(TRIGGER_EVENT_CHANNEL);
    beepEvent = syncMgr->event<void>(BEEP_EVENT_CHANNEL);

    triggerEvent.onEvent([&](uint32_t from, uint8_t v)
                         {
                           lastTriggerFrom = from;
                           lastTriggerValue = v;
                           lastTriggerTime = millis();
                           Serial.printf("Trigger received from %08X: %d\n", from, v);
                         });

    Serial.printf("Send opened registered\n");
  }

  void onExit()
  {
    syncMgr->unregisterEvent(BEEP_EVENT_CHANNEL);
    beepEvent = EventHandle<void>();

    Serial.printf("Send closed unregistered\n");
  }

} // namespace SendScreenNamespace

const Screen2 SendScreen = {
    .name = "Send",
    .draw = SendScreenNamespace::draw,
    .update = SendScreenNamespace::update,
    .onEnter = SendScreenNamespace::onEnter,
    .onExit = SendScreenNamespace::onExit,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
