#include "CarFlash.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Wireless.h"
#include "ScreenManager.h"

namespace CarFlashScreenNamespace
{

  namespace
  {
    static Menu menu;
    static MenuItemBack backItem;

    static uint8_t count = 5;
    static uint8_t delay = 50;

    static MenuItemAction relay1FlashItem{"Flash R 1", 2, [&]()
                                          {
                                            TransportPacket p;
                                            p.type = CMD_RELAY_1_FLASH;
                                            p.len = 2;
                                            p.data[0] = count;
                                            p.data[1] = uint8_t(delay / 10);

                                            wireless.send(&p, car_addr);
                                          }};

    static MenuItemNumber<uint8_t> relay1FlashCountItem{"Count", &count, 1, 100};
    static MenuItemNumber<uint8_t> relay1FlashDelayItem{"Delay", &delay, 20, 250, 10};

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      menu.addMenuItem(&backItem);
      menu.addMenuItem(&relay1FlashItem);
      menu.addMenuItem(&relay1FlashCountItem);
      menu.addMenuItem(&relay1FlashDelayItem);
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

} // namespace CarFlashScreenNamespace

const Screen2 CarFlashScreen = {
    .name = "CarFlash",
    .draw = CarFlashScreenNamespace::draw,
    .update = CarFlashScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
