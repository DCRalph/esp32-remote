#pragma once

#include <Menu.h>

#include "config.h"
#include "Screens/Screens.h"

namespace
{
  Menu switchMenu(MenuSize::Large);

  MenuItemBack switchMenuBackItem;
  MenuItemNavigate switchMenuCarItem("Car", &CarControlScreen2);
  MenuItemNavigate switchMenuRelayItem("Relay", &RemoteRelayScreen2);
  MenuItemNavigate switchMenuEncoderItem("Encoder", &EncoderTransmiterScreen2);
  MenuItemNavigate switchMenuServosItem("Servos", &ServoControlScreen2);

  [[maybe_unused]] const bool switchMenuBuilt = []()
  {
    switchMenu.addMenuItem(&switchMenuBackItem);
    switchMenu.addMenuItem(&switchMenuCarItem);
    switchMenu.addMenuItem(&switchMenuRelayItem);
    switchMenu.addMenuItem(&switchMenuEncoderItem);
    switchMenu.addMenuItem(&switchMenuServosItem);
    return true;
  }();

  void switchMenuDraw()
  {
    switchMenu.draw();
  }

  void switchMenuUpdate()
  {
    switchMenu.update();
  }
}

const Screen2 SwitchMenuScreen2 = {
    .name = "Control",
    .draw = switchMenuDraw,
    .update = switchMenuUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
