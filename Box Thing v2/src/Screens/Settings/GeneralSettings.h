#pragma once

#include <Menu.h>

#include "config.h"
#include "Screens/Screens.h"

namespace
{
  Menu generalSettingsMenu(MenuSize::Large);

  MenuItemBack generalSettingsBackItem;
  MenuItemNumber<int> generalSettingsAutoOffItem("Auto Off", &autoOffMin, 0, 60);

  [[maybe_unused]] const bool generalSettingsMenuBuilt = []()
  {
    generalSettingsMenu.addMenuItem(&generalSettingsBackItem);
    generalSettingsMenu.addMenuItem(&generalSettingsAutoOffItem);

    generalSettingsAutoOffItem.setOnChange([]()
                                           {
                                             preferences.putInt("autoOffMin", autoOffMin);
                                             debugI("Auto off time: %d min", autoOffMin);
                                             //
                                           });
    return true;
  }();

  void generalSettingsDraw()
  {
    generalSettingsMenu.draw();
  }

  void generalSettingsUpdate()
  {
    generalSettingsMenu.update();
  }
}

const Screen2 GeneralSettingsScreen2 = {
    .name = "General",
    .draw = generalSettingsDraw,
    .update = generalSettingsUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
