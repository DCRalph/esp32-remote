#include "screens/Screens.h"

#include <WiFi.h>
#include <dcr_NetLink.h>
#include <esp_sleep.h>

#include "config.h"
#include "Displays.h"
#include "IO/Battery.h"
#include "IO/Touch.h"

namespace
{
  Menu menu(MenuSize::Small);
  String battText, wifiText, ssidText, ipText, heapText, psramText, uptimeText;

  MenuItemBack backItem;
  MenuItemString battItem("Battery", &battText);
  MenuItemString wifiItem("Wi-Fi", &wifiText);
  MenuItemString ssidItem("SSID", &ssidText);
  MenuItemString ipItem("IP", &ipText);
  MenuItemString heapItem("Heap free", &heapText);
  MenuItemString psramItem("PSRAM free", &psramText);
  MenuItemString uptimeItem("Uptime", &uptimeText);
  MenuItemNavigate touchCalItem("Touch cal", &TouchCalScreen);
  MenuItemAction touchResetItem("Reset touch cal", 1, []()
                                {
                                  Touch::reset();
                                  screenManager.showPopup(new AutoClosePopup("Touch", "Calibration reset", 1500)); });
  MenuItemAction powerItem("Power off", 1, []()
                           {
                             digitalWrite(POWER_ON_PIN, LOW);
                             esp_deep_sleep_start(); });

  uint32_t lastRefreshMs = 0;

  void refresh()
  {
    battText = String(battery.getVoltage(), 2) + "V " + battery.getPercentageI() + "%";
    wifiText = netLink.isConnected() ? "connected" : "offline";
    ssidText = netLink.isConnected() ? WiFi.SSID() : String("-");
    ipText = netLink.isConnected() ? WiFi.localIP().toString() : String("-");
    heapText = String(ESP.getFreeHeap() / 1024) + " KB";
    psramText = String(ESP.getFreePsram() / 1024) + " KB";
    uptimeText = String(millis() / 1000) + " s";
  }

  void enter()
  {
    if (menu.items.empty())
    {
      for (MenuItem *item : {static_cast<MenuItem *>(&backItem), static_cast<MenuItem *>(&battItem),
                             static_cast<MenuItem *>(&wifiItem), static_cast<MenuItem *>(&ssidItem),
                             static_cast<MenuItem *>(&ipItem), static_cast<MenuItem *>(&heapItem),
                             static_cast<MenuItem *>(&psramItem), static_cast<MenuItem *>(&uptimeItem),
                             static_cast<MenuItem *>(&touchCalItem), static_cast<MenuItem *>(&touchResetItem),
                             static_cast<MenuItem *>(&powerItem)})
        menu.addMenuItem(item);
    }
    refresh();
  }

  void update()
  {
    if (millis() - lastRefreshMs > 500)
    {
      lastRefreshMs = millis();
      refresh();
    }
    menu.update();
  }
}

const Screen2 SettingsScreen = {
    .name = "Settings",
    .draw = []()
    { menu.draw(); },
    .update = update,
    .onEnter = enter,
    .onExit = nullptr,
    .topBarColor = Color::Red,
    .topBarTextColor = Color::White,
};
