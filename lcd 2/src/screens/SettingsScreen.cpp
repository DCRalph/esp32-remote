#include "SettingsScreen.h"

#include "config.h"
#include <WiFi.h>
#include <esp_wifi.h>

#include "Display.h"
#include "Menu.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

#include "Settings/batteryScreen.h"
#include "Settings/MeshScreen.h"
#include "Settings/RSSIMeter.h"
#include "Settings/SystemInfoScreen.h"
#include "Settings/WiFiInfo.h"

namespace SettingsScreenNamespace
{

  namespace
  {
    static long bootCount = 0;

    static Menu menu;
    static MenuItemBack backItem;

    static MenuItemNavigate batteryItem{"Battery", &BatteryScreen};
    static MenuItemNavigate systemInfoItem{"System Info", &SystemInfoScreen};
    static MenuItemNavigate wifiItem{"Wi-Fi Info", &WiFiInfoScreen};
    static MenuItemNavigate meshItem{"Mesh", &MeshScreen};
    static MenuItemNavigate rssiItem{"RSSI", &RSSIMeter};
    static MenuItemNumber<long> bootCountItem{"Boot Count", &bootCount};

    static MenuItemAction serialMacAddrItem{"Serial MAC Address", 2, [&]()
                                            {
                                              uint8_t baseMac[6];
                                              esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
                                              if (ret == ESP_OK)
                                              {
                                                Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                                                              baseMac[0], baseMac[1], baseMac[2],
                                                              baseMac[3], baseMac[4], baseMac[5]);
                                              }
                                              else
                                              {
                                                Serial.println("Failed to read MAC address");
                                              }
                                            }};

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      menu.addMenuItem(&backItem);
      menu.addMenuItem(&batteryItem);
      menu.addMenuItem(&systemInfoItem);
      menu.addMenuItem(&wifiItem);
      menu.addMenuItem(&meshItem);
      menu.addMenuItem(&rssiItem);
      menu.addMenuItem(&bootCountItem);
      menu.addMenuItem(&serialMacAddrItem);
    }
  }

  void onEnter()
  {
    ensureInitialized();
    bootCount = preferences.getLong("bootCount", 0);
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

} // namespace SettingsScreenNamespace

const Screen2 SettingsScreen = {
    .name = "Settings",
    .draw = SettingsScreenNamespace::draw,
    .update = SettingsScreenNamespace::update,
    .onEnter = SettingsScreenNamespace::onEnter,
    .onExit = nullptr,
    .topBarColor = TFT_RED,
    .topBarTextColor = TFT_WHITE,
};
