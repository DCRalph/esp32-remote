#include "RemoteRelay.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "ScreenManager.h"
#include "Wireless.h"
#include "IO/GPIO.h"
#include "IO/Buttons.h"

namespace
{

  void relayChangeCb(uint8_t cmd, bool *global)
  {
    TransportPacket pkt{};
    pkt.type = cmd;
    pkt.len = 1;
    pkt.data[0] = static_cast<uint8_t>(!*global);

    wireless.send(&pkt, remote_addr);
  }

} // namespace

namespace RemoteRelayScreenNamespace
{

  namespace
  {
    static Menu menu;
    static MenuItemBack backItem;
    static MenuItemToggle relay1Item{"R 1", &globalRelay1, false};
    static MenuItemToggle relay2Item{"R 2", &globalRelay2, false};
    static MenuItemToggle relay3Item{"R 3", &globalRelay3, false};
    static MenuItemToggle relay4Item{"R 4", &globalRelay4, false};
    static MenuItemToggle relay5Item{"R 5", &globalRelay5, false};
    static MenuItemToggle relay6Item{"R 6", &globalRelay6, false};
    static MenuItemToggle relay7Item{"R 7", &globalRelay7, false};
    static MenuItemToggle relay8Item{"R 8", &globalRelay8, false};

    static uint64_t lastFetch = 0;

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      menu.addMenuItem(&backItem);

      menu.addMenuItem(&relay1Item);
      relay1Item.setTextColor(TFT_RED);

      menu.addMenuItem(&relay2Item);
      relay2Item.setTextColor(TFT_BLUE);

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

    if (millis() - lastFetch > 1000)
    {
      lastFetch = millis();

      TransportPacket pkt{};
      pkt.type = CMD_RELAY_ALL;
      pkt.len = 0;

      wireless.send(&pkt, remote_addr);
    }

    if (menu.items[menu.getActive()] == &relay1Item)
    {
      btnLed.SetColor(255, 0, 0);
      if (ClickButtonTRIGGER.clicks == 1)
        relay1Item.executeFunc(2);
    }

    if (menu.items[menu.getActive()] == &relay2Item)
    {
      btnLed.SetColor(0, 0, 255);
      if (ClickButtonTRIGGER.clicks == 1)
        relay2Item.executeFunc(2);
    }

    if (menu.items[menu.getActive()] != &relay1Item && menu.items[menu.getActive()] != &relay2Item)
    {
      btnLed.Off();
    }
  }

  void onEnter()
  {
    ensureInitialized();
    TransportPacket pkt{};
    pkt.type = CMD_RELAY_ALL;
    pkt.len = 0;

    wireless.send(&pkt, remote_addr);
  }

  void onExit()
  {
    btnLed.Off();
  }

} // namespace RemoteRelayScreenNamespace

const Screen2 RemoteRelayScreen = {
    .name = "RemoteRelay",
    .draw = RemoteRelayScreenNamespace::draw,
    .update = RemoteRelayScreenNamespace::update,
    .onEnter = RemoteRelayScreenNamespace::onEnter,
    .onExit = RemoteRelayScreenNamespace::onExit,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
