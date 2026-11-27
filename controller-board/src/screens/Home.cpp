#include "screens/Screens.h"

#include "Device.h"

namespace
{
  Menu menu(MenuSize::Large);
  MenuItemGroup testGroup("Test"), hardwareGroup("Hardware"), powerGroup("Power");
  MenuItemNavigate uiTestItem("UI test", &UiTest::RootScreen);
  MenuItemNavigate touchTestItem("Touch test", &TouchTestScreen);
  MenuItemNavigate calibrateItem("Calibrate touch", &TouchCalScreen);
  MenuItemNavigate backlightItem("Backlight", &BacklightScreen);
  MenuItemNavigate buttonsItem("Buttons", &ButtonsScreen);
  MenuItemNavigate deviceItem("Device", &DeviceScreen);
  MenuItemAction powerItem("Power off", 1, []()
                           { Device::powerOff(); });

  void enter()
  {
    if (!menu.items.empty())
      return;
    for (MenuItem *item : std::initializer_list<MenuItem *>{&testGroup, &uiTestItem, &touchTestItem, &calibrateItem,
                                                            &hardwareGroup, &backlightItem, &buttonsItem, &deviceItem,
                                                            &powerGroup, &powerItem})
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
