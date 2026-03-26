#include "Car.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Wireless.h"
#include "ScreenManager.h"
#include "CarFlash.h"

namespace CarScreenNamespace
{

  static void relayChangeCb(uint8_t cmd, bool *global)
  {
    WirelessFrame fp;
    fp.direction = PacketDirection::SEND;
    fp.packet.type = cmd;
    fp.packet.data[0] = !*global;

    wireless.sendPacket(fp.packet, TransportAddress::fromMac(car_addr));
  }

  namespace
  {
    static Menu menu;
    static MenuItemBack backItem;

    static MenuItemAction lockDoorItem{"Lock Door", 2, []()
                                       {
                                         TransportPacket p;
                                         p.type = CMD_DOOR_LOCK;
                                         p.len = 1;
                                         p.data[0] = 0;
                                         wireless.send(&p, car_addr);
                                       }};

    static MenuItemAction unlockDoorItem{"Unlock Door", 2, []()
                                         {
                                           TransportPacket p;
                                           p.type = CMD_DOOR_LOCK;
                                           p.len = 1;
                                           p.data[0] = 1;
                                           wireless.send(&p, car_addr);
                                         }};

    static MenuItemNavigate carFlashItem{"Flash", &CarFlashScreen};

    static MenuItemToggle relay1Item{"R 1", &globalRelay1, false};
    static MenuItemToggle relay2Item{"R 2", &globalRelay2, false};
    static MenuItemToggle relay3Item{"R 3", &globalRelay3, false};
    static MenuItemToggle relay4Item{"R 4", &globalRelay4, false};
    static MenuItemToggle relay5Item{"R 5", &globalRelay5, false};
    static MenuItemToggle relay6Item{"R 6", &globalRelay6, false};

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

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
    WirelessFrame fp;
    TransportAddress source = TransportAddress::fromMac(car_addr);
    fp.direction = PacketDirection::SEND;
    fp.packet.type = CMD_RELAY_ALL;
    fp.packet.len = 0;

    wireless.sendPacket(fp.packet, TransportAddress::fromMac(car_addr));
  }

} // namespace CarScreenNamespace

const Screen2 CarScreen = {
    .name = "Car",
    .draw = CarScreenNamespace::draw,
    .update = CarScreenNamespace::update,
    .onEnter = CarScreenNamespace::onEnter,
    .onExit = nullptr,
    .topBarColor = TFT_WHITE,
    .topBarTextColor = TFT_BLACK,
};
