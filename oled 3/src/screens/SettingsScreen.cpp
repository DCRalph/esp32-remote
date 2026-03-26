#include "SettingsScreen.h"

#include "config.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include "Display.h"
#include "Menu.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"
#include "IO/GPIO.h"

#include "Settings/batteryScreen.h"
#include "Settings/SystemInfoScreen.h"
#include "Settings/WiFiInfo.h"
#include "Settings/RSSIMeter.h"
#include "Settings/MeshScreen.h"

namespace
{
  static const char *TAG = "Settings";
}

namespace SettingsScreenNamespace
{

  namespace
  {
    static long bootCount = 0;
    static bool rainbowMode = false;

    static Menu menu;
    static MenuItemBack backItem;

    static MenuItemNavigate batteryItem{"Battery", &BatteryScreen};
    static MenuItemNavigate systemInfoItem{"System Info", &SystemInfoScreen};
    static MenuItemNavigate wifiItem{"Wi-Fi Info", &WiFiInfoScreen};
    static MenuItemNavigate rssiItem{"RSSI", &RSSIMeter};
    static MenuItemNavigate meshItem{"Mesh", &MeshScreen};

    static MenuItemToggle rainbowModeItem{"Rainbow Mode", &rainbowMode, false};

    static MenuItemNumber<long> bootCountItem{"Boot Count", &bootCount};

    static MenuItemAction serialMacAddrItem{"Serial MAC Address", 2, [&]()
                                            {
                                              uint8_t baseMac[6];
                                              esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
                                              if (ret == ESP_OK)
                                              {
                                                ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x\n",
                                                         baseMac[0], baseMac[1], baseMac[2],
                                                         baseMac[3], baseMac[4], baseMac[5]);
                                              }
                                              else
                                              {
                                                ESP_LOGW(TAG, "Failed to read MAC address");
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
      menu.addMenuItem(&rssiItem);
      menu.addMenuItem(&meshItem);
      menu.addMenuItem(&rainbowModeItem);
      menu.addMenuItem(&bootCountItem);
      menu.addMenuItem(&serialMacAddrItem);

      rainbowModeItem.setOnChange([&]()
                                  {
                                    btnLed.SetMode(rainbowMode ? RGB_MODE::Manual : RGB_MODE::Rainbow);
                                  });
    }
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

    rainbowMode = btnLed.GetMode() == RGB_MODE::Rainbow;
  }

  void onEnter()
  {
    ensureInitialized();
    btnLed.SetColor565(TFT_RED);
    bootCount = preferences.getLong("bootCount", 0);
  }

  void onExit()
  {
    btnLed.Off();
  }

} // namespace SettingsScreenNamespace

const Screen2 SettingsScreen = {
    .name = "Settings",
    .draw = SettingsScreenNamespace::draw,
    .update = SettingsScreenNamespace::update,
    .onEnter = SettingsScreenNamespace::onEnter,
    .onExit = SettingsScreenNamespace::onExit,
    .topBarColor = TFT_RED,
    .topBarTextColor = TFT_WHITE,
};
