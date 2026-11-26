#include "RemoteRelay.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "ScreenManager.h"
#include "IO/GPIO.h"
#include "IO/Buttons.h"

namespace
{
  using namespace RemoteProtocol::CarRelays;

  constexpr uint32_t kReplyTimeoutMs = 300;

  void onRelays(bool ok, const Relays &relays)
  {
    if (ok)
      applyCarRelays(relays);
  }

  // The toggles are read-only: a press asks the car, and its reply sets the state.
  void requestRelay(uint8_t index)
  {
    set().request(remote_addr, {index, !carRelays[index]}, kReplyTimeoutMs, onRelays);
  }

} // namespace

namespace RemoteRelayScreenNamespace
{

  namespace
  {
    static Menu menu;
    static MenuItemBack backItem;
    static MenuItemToggle relayItems[kRelayCount] = {
        {"R 1", &carRelays[0], false},
        {"R 2", &carRelays[1], false},
        {"R 3", &carRelays[2], false},
        {"R 4", &carRelays[3], false},
        {"R 5", &carRelays[4], false},
        {"R 6", &carRelays[5], false},
    };
    static MenuItemToggle &relay1Item = relayItems[0];
    static MenuItemToggle &relay2Item = relayItems[1];

    static uint64_t lastFetch = 0;

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      menu.addMenuItem(&backItem);

      for (uint8_t i = 0; i < kRelayCount; i++)
      {
        menu.addMenuItem(&relayItems[i]);
        relayItems[i].setOnChange([i]()
                                  { requestRelay(i); });
      }
      relay1Item.setTextColor(TFT_RED);
      relay2Item.setTextColor(TFT_BLUE);
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
      get().request(remote_addr, {}, kReplyTimeoutMs, onRelays);
    }

    if (menu.items[menu.getActive()] == &relay1Item)
    {
      btnLed.SetColor(255, 0, 0);
      if (btnTrigger.clicks() == 1)
        relay1Item.executeFunc(2);
    }

    if (menu.items[menu.getActive()] == &relay2Item)
    {
      btnLed.SetColor(0, 0, 255);
      if (btnTrigger.clicks() == 1)
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
    get().request(remote_addr, {}, kReplyTimeoutMs, onRelays);
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
