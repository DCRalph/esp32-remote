#pragma once

#include <Menu.h>
#include <dcr_NetLink.h>

#include "config.h"
#include "Screens/Screens.h"

namespace
{
  bool gWifiSettingsWifiActive = false;
  bool gWifiSettingsEspnowActive = false;

  Menu wifiSettingsMenu;

  MenuItemBack wifiSettingsBackItem;
  MenuItemNavigate wifiSettingsInfoItem("WiFi Info", &WiFiInfoScreen2);
  MenuItemNavigate wifiSettingsScanItem("Add Network", &WiFiScanScreen2);
  MenuItemToggle wifiSettingsWifiItem("WiFi", &gWifiSettingsWifiActive);
  MenuItemToggle wifiSettingsEspnowItem("ESPNOW", &gWifiSettingsEspnowActive);
  MenuItemAction wifiSettingsForgetItem("Reset WiFi", 1, []()
                                        { netLink.resetConfig(); });

  [[maybe_unused]] const bool wifiSettingsMenuBuilt = []()
  {
    wifiSettingsMenu.addMenuItem(&wifiSettingsBackItem);
    wifiSettingsMenu.addMenuItem(&wifiSettingsInfoItem);
    wifiSettingsMenu.addMenuItem(&wifiSettingsScanItem);
    wifiSettingsMenu.addMenuItem(&wifiSettingsWifiItem);
    wifiSettingsMenu.addMenuItem(&wifiSettingsEspnowItem);
    wifiSettingsMenu.addMenuItem(&wifiSettingsForgetItem);

    // WiFi and ESP-NOW both own the radio, so enabling one tears down the other.
    wifiSettingsWifiItem.setOnChange([]()
                                     {
                                       if (WiFi.status() == WL_CONNECTED)
                                       {
                                         netLink.off();
                                         return;
                                       }

                                       if (wireless.isSetupDone())
                                       {
                                         teardownMeshTransport();
                                         preferences.putBool("espnowOn", false);
                                       }

                                       netLink.on();
                                       netLink.connect();
                                       //
                                     });

    wifiSettingsEspnowItem.setOnChange([]()
                                       {
                                         if (wireless.isSetupDone())
                                         {
                                           teardownMeshTransport();
                                           preferences.putBool("espnowOn", false);
                                           return;
                                         }

                                         netLink.prepareForExternalTransport();
                                         beginMeshTransport();
                                         preferences.putBool("espnowOn", true);
                                         //
                                       });
    return true;
  }();

  void wifiSettingsSyncToggles()
  {
    wifiSettingsWifiItem.set(WiFi.status() == WL_CONNECTED);
    wifiSettingsEspnowItem.set(wireless.isSetupDone());
  }

  void wifiSettingsDraw()
  {
    wifiSettingsMenu.draw();
  }

  void wifiSettingsUpdate()
  {
    wifiSettingsMenu.update();
    wifiSettingsSyncToggles();
  }
}

const Screen2 WiFiSettingsScreen2 = {
    .name = "Wi-Fi",
    .draw = wifiSettingsDraw,
    .update = wifiSettingsUpdate,
    .onEnter = wifiSettingsSyncToggles,
    .onExit = nullptr,
};
