#pragma once

#include <Menu.h>
#include <ScreenManager.h>

#include "config.h"
#include "Screens/Screens.h"

namespace
{
  Menu homeMenu(MenuSize::Large);

  MenuItemNavigate homeSwitchMenuItem("Control", &SwitchMenuScreen2);
  MenuItemNavigate homeSettingsItem("Settings", &SettingsScreen2);
  MenuItemAction homePowerOffItem("Power Off", -1, []()
                                  { screenManager.setScreen(&ShutdownScreen2); });

  [[maybe_unused]] const bool homeMenuBuilt = []()
  {
    homeMenu.addMenuItem(&homeSwitchMenuItem);
    homeMenu.addMenuItem(&homeSettingsItem);
    homeMenu.addMenuItem(&homePowerOffItem);

    homeSettingsItem.addRoute(2, &DebugScreen2);
    return true;
  }();

  void homeDraw()
  {
    homeMenu.draw();
  }

  void homeUpdate()
  {
    homeMenu.update();
  }
}

const Screen2 HomeScreen2 = {
    .name = "Home",
    .draw = homeDraw,
    .update = homeUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
