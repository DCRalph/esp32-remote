#include "Control.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Wireless.h"
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
                                           {
                                             TransportPacket p;
                                             p.type = 0x30;
                                             p.len = 2;
                                             p.data[0] = 5;
                                             p.data[1] = 30 / 10;
                                             wireless.sendPacket(p, TransportAddress::fromMac(car_addr));
                                           }};
    static MenuItemAction flashItemPreset2{"Flash 2 40", 2, [&]()
                                           {
                                             TransportPacket p;
                                             p.type = 0x30;
                                             p.len = 2;
                                             p.data[0] = 2;
                                             p.data[1] = 40 / 10;
                                             wireless.sendPacket(p, TransportAddress::fromMac(car_addr));
                                           }};
    static MenuItemAction flashItemPreset3{"Flash 8 50", 2, [&]()
                                           {
                                             TransportPacket p;
                                             p.type = 0x30;
                                             p.len = 2;
                                             p.data[0] = 8;
                                             p.data[1] = 50 / 10;
                                             wireless.sendPacket(p, TransportAddress::fromMac(car_addr));
                                           }};

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
