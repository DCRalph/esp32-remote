
#include <Menu.h>
#include <RemoteProtocol.h>

#include "config.h"
#include "Screens/Screens.h"

namespace
{
  uint64_t gRelayLastPing = 0;
  uint64_t gRelayLastConfirmedPing = 0;
  bool gRelayConnected = false;
  bool gRelayLastConnected = false;
  bool gRelayArmed = false;

  void relayFire(int relay)
  {
    if (!gRelayConnected || !gRelayArmed)
      return;

    RemoteProtocol::FireBox::fire().sendTo(remote_addr, {static_cast<uint8_t>(1u << relay)});
  }

  Menu relayMenu;

  MenuItemBack relayBackItem;
  MenuItemToggle relayConnectionItem("Conn", &gRelayConnected, false);
  MenuItemToggle relayArmItem("Armed", &gRelayArmed);
  MenuItemAction relayTestItem("Test", 1, []()
                               { RemoteProtocol::FireBox::test().sendTo(remote_addr, {}); });

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
                             { RemoteProtocol::FireBox::arm().sendTo(remote_addr, {gRelayArmed}); });
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
      RemoteProtocol::FireBox::ping().request(remote_addr, {}, 200, [](bool ok, const RemoteProtocol::FireBox::Pong &)
                                              {
                                                if (ok)
                                                  gRelayLastConfirmedPing = millis();
                                              });
    }
  }
}

const Screen2 RemoteRelayScreen2 = {
    .name = "Relay",
    .draw = relayDraw,
    .update = relayUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
