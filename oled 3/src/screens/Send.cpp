#include "Send.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

namespace SendScreenNamespace
{

  namespace
  {
    static Menu menu;
    static MenuItemBack backItem;

    static u8_t peer_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};

    static MenuItemAction sendItem{"Send", 2, [&]()
                                   { RemoteProtocol::Hello::message().sendTo(peer_addr, RemoteProtocol::Hello::Message{"Hello"}); }};

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      menu.addMenuItem(&backItem);
      menu.addMenuItem(&sendItem);
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

} // namespace SendScreenNamespace

const Screen2 SendScreen = {
    .name = "Send",
    .draw = SendScreenNamespace::draw,
    .update = SendScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
