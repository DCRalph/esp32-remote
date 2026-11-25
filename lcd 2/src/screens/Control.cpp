#include "Control.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "ScreenManager.h"
#include "Control/Car.h"

namespace ControlScreenNamespace
{

  namespace
  {
    static Menu menu;
    static MenuItemBack backItem;
    static MenuItemNavigate carItem{"Car Control", &CarScreen};
    static MenuItemAction flashItemPreset1{"Flash 5 30", 2, [&]()
                                           { RemoteProtocol::CarRelays::flash().sendTo(car_addr, {5, 30}); }};
    static MenuItemAction flashItemPreset2{"Flash 2 40", 2, [&]()
                                           { RemoteProtocol::CarRelays::flash().sendTo(car_addr, {2, 40}); }};
    static MenuItemAction flashItemPreset3{"Flash 8 50", 2, [&]()
                                           { RemoteProtocol::CarRelays::flash().sendTo(car_addr, {8, 50}); }};

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      menu.addMenuItem(&backItem);
      menu.addMenuItem(&carItem);
      menu.addMenuItem(&flashItemPreset1);
      menu.addMenuItem(&flashItemPreset2);
      menu.addMenuItem(&flashItemPreset3);
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

} // namespace ControlScreenNamespace

const Screen2 ControlScreen = {
    .name = "Control",
    .draw = ControlScreenNamespace::draw,
    .update = ControlScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_CYAN,
    .topBarTextColor = TFT_BLACK,
};
