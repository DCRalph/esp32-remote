#include "Car.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "ScreenManager.h"
#include "CarFlash.h"

namespace CarScreenNamespace
{

  using namespace RemoteProtocol::CarRelays;

  constexpr uint32_t kReplyTimeoutMs = 300;

  void onRelays(bool ok, const Relays &relays)
  {
    if (ok)
      applyCarRelays(relays);
  }

  // The toggles are read-only: a press asks the car, and its reply sets the state.
  static void requestRelay(uint8_t index)
  {
    set().request(car_addr, {index, !carRelays[index]}, kReplyTimeoutMs, onRelays);
  }

  namespace
  {
    static Menu menu;
    static MenuItemBack backItem;

    static MenuItemAction lockDoorItem{"Lock Door", 2, []()
                                       { lock().sendTo(car_addr, {0}); }};

    static MenuItemAction unlockDoorItem{"Unlock Door", 2, []()
                                         { lock().sendTo(car_addr, {1}); }};

    static MenuItemNavigate carFlashItem{"Flash", &CarFlashScreen};

    static MenuItemToggle relayItems[kRelayCount] = {
        {"R 1", &carRelays[0], false},
        {"R 2", &carRelays[1], false},
        {"R 3", &carRelays[2], false},
        {"R 4", &carRelays[3], false},
        {"R 5", &carRelays[4], false},
        {"R 6", &carRelays[5], false},
    };

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
      for (uint8_t i = 0; i < kRelayCount; i++)
      {
        menu.addMenuItem(&relayItems[i]);
        relayItems[i].setOnChange([i]()
                                  { requestRelay(i); });
      }
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
    get().request(car_addr, {}, kReplyTimeoutMs, onRelays);
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
