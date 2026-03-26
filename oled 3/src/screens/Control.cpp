#include "Control.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Wireless.h"
#include "ScreenManager.h"
#include "IO/GPIO.h"
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
                                             TransportPacket pkt{};
                                             pkt.type = 0x30;
                                             pkt.len = 2;
                                             pkt.data[0] = 5;
                                             pkt.data[1] = 30 / 10;
                                             wireless.send(&pkt, remote_addr);
                                           }};

    static MenuItemAction flashItemPreset2{"Flash 2 40", 2, [&]()
                                           {
                                             TransportPacket pkt{};
                                             pkt.type = 0x30;
                                             pkt.len = 2;
                                             pkt.data[0] = 2;
                                             pkt.data[1] = 40 / 10;
                                             wireless.send(&pkt, remote_addr);
                                           }};

    static MenuItemAction flashItemPreset3{"Flash 8 50", 2, [&]()
                                           {
                                             TransportPacket pkt{};
                                             pkt.type = 0x30;
                                             pkt.len = 2;
                                             pkt.data[0] = 8;
                                             pkt.data[1] = 50 / 10;
                                             wireless.send(&pkt, remote_addr);
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

  void onEnter()
  {
    ensureInitialized();
    btnLed.SetColor565(TFT_CYAN);
  }

  void onExit()
  {
    btnLed.Off();
  }

} // namespace ControlScreenNamespace

const Screen2 ControlScreen = {
    .name = "Control",
    .draw = ControlScreenNamespace::draw,
    .update = ControlScreenNamespace::update,
    .onEnter = ControlScreenNamespace::onEnter,
    .onExit = ControlScreenNamespace::onExit,
    .topBarColor = TFT_CYAN,
    .topBarTextColor = TFT_BLACK,
};
