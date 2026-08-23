#pragma once

#include <Menu.h>
#include <Wireless.h>

#include "config.h"
#include "Screens/Screens.h"

namespace RemoteRelay
{
  /** Called from the ESP-NOW receive hook when the relay board answers a ping. */
  void notePingReply();
}

namespace
{
  uint64_t gRelayLastPing = 0;
  uint64_t gRelayLastConfirmedPing = 0;
  bool gRelayConnected = false;
  bool gRelayLastConnected = false;
  bool gRelayArmed = false;

  /** Send a zero-payload command to the relay board. */
  void relaySendCommand(uint8_t type)
  {
    wireless.sendTyped(type, remote_addr);
  }

  void relayFire(int relay)
  {
    if (!gRelayConnected || !gRelayArmed)
      return;

    bool relays[8] = {false, false, false, false, false, false, false, false};
    relays[relay] = true;

    wireless.sendTyped(CMD_FIRE, relays, remote_addr);
  }

  Menu relayMenu;

  MenuItemBack relayBackItem;
  MenuItemToggle relayConnectionItem("Conn", &gRelayConnected, false);
  MenuItemToggle relayArmItem("Armed", &gRelayArmed);
  MenuItemAction relayTestItem("Test", 1, []()
                               { relaySendCommand(CMD_TEST); });

  MenuItemAction relayFire1Item("Fire 1", 1, []()
                                { relayFire(0); });
  MenuItemAction relayFire2Item("Fire 2", 1, []()
                                { relayFire(1); });
  MenuItemAction relayFire3Item("Fire 3", 1, []()
                                { relayFire(2); });
  MenuItemAction relayFire4Item("Fire 4", 1, []()
                                { relayFire(3); });
  MenuItemAction relayFire5Item("Fire 5", 1, []()
                                { relayFire(4); });
  MenuItemAction relayFire6Item("Fire 6", 1, []()
                                { relayFire(5); });
  MenuItemAction relayFire7Item("Fire 7", 1, []()
                                { relayFire(6); });
  MenuItemAction relayFire8Item("Fire 8", 1, []()
                                { relayFire(7); });

  [[maybe_unused]] const bool relayMenuBuilt = []()
  {
    relayMenu.addMenuItem(&relayBackItem);

    relayMenu.addMenuItem(&relayConnectionItem);
    relayMenu.addMenuItem(&relayArmItem);
    relayMenu.addMenuItem(&relayTestItem);

    relayMenu.addMenuItem(&relayFire1Item);
    relayMenu.addMenuItem(&relayFire2Item);
    relayMenu.addMenuItem(&relayFire3Item);
    relayMenu.addMenuItem(&relayFire4Item);
    relayMenu.addMenuItem(&relayFire5Item);
    relayMenu.addMenuItem(&relayFire6Item);
    relayMenu.addMenuItem(&relayFire7Item);
    relayMenu.addMenuItem(&relayFire8Item);

    relayArmItem.setOnChange([]()
                             { relaySendCommand(gRelayArmed ? CMD_ARM : CMD_DISARM); });
    return true;
  }();

  void relayDraw()
  {
    relayMenu.draw();
  }

  void relayUpdate()
  {
    relayMenu.update();

    gRelayConnected = millis() - gRelayLastConfirmedPing < 1000;

    if (gRelayConnected != gRelayLastConnected)
    {
      gRelayLastConnected = gRelayConnected;

      // Never leave the board armed once the link drops.
      if (!gRelayConnected)
        gRelayArmed = false;
    }

    if (millis() - gRelayLastPing > 200)
    {
      gRelayLastPing = millis();
      relaySendCommand(CMD_PING);
    }
  }
}

void RemoteRelay::notePingReply()
{
  gRelayLastConfirmedPing = millis();
}

const Screen2 RemoteRelayScreen2 = {
    .name = "Relay",
    .draw = relayDraw,
    .update = relayUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
