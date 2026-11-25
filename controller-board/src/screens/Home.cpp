#include "screens/Screens.h"

#include "Device.h"

namespace
{
  Menu menu(MenuSize::Large);
  MenuItemNavigate uiTestItem("UI test", &UiTest::RootScreen);
  MenuItemNavigate touchTestItem("Touch test", &TouchTestScreen);
  MenuItemNavigate calibrateItem("Calibrate touch", &TouchCalScreen);
  MenuItemNavigate backlightItem("Backlight", &BacklightScreen);
  MenuItemNavigate deviceItem("Device", &DeviceScreen);
  MenuItemAction powerItem("Power off", 1, []()
                           { Device::powerOff(); });

  void enter()
  {
    if (!menu.items.empty())
      return;
    for (MenuItem *item : {static_cast<MenuItem *>(&uiTestItem), static_cast<MenuItem *>(&touchTestItem),
                           static_cast<MenuItem *>(&calibrateItem), static_cast<MenuItem *>(&backlightItem),
                           static_cast<MenuItem *>(&deviceItem),
                           static_cast<MenuItem *>(&powerItem)})
      menu.addMenuItem(item);
  }
}

const Screen2 HomeScreen = {
    .name = "Controller board",
    .draw = []()
    { menu.draw(); },
    .update = []()
    { menu.update(); },
    .onEnter = enter,
    .onExit = nullptr,
};
