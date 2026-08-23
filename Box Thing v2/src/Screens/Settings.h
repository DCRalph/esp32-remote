#pragma once

#include <Menu.h>

#include "config.h"
#include "Screens/Screens.h"

namespace
{
  Menu settingsMenu(MenuSize::Large);

  MenuItemBack settingsBackItem;
  MenuItemNavigate settingsGeneralItem("General", &GeneralSettingsScreen2);
  MenuItemNavigate settingsWifiItem("WiFi", &WiFiSettingsScreen2);

  [[maybe_unused]] const bool settingsMenuBuilt = []()
  {
    settingsMenu.addMenuItem(&settingsBackItem);
    settingsMenu.addMenuItem(&settingsGeneralItem);
    settingsMenu.addMenuItem(&settingsWifiItem);
    return true;
  }();

  void settingsDraw()
  {
    settingsMenu.draw();
  }

  void settingsUpdate()
  {
    settingsMenu.update();
  }
}

const Screen2 SettingsScreen2 = {
    .name = "Settings",
    .draw = settingsDraw,
    .update = settingsUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
