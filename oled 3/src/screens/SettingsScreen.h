#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"

static const char *TAG = "Settings";

class Settings : public Screen
{
public:
  Settings(String _name);

  long bootCount;
  bool rainbowMode = false;

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate batteryItem = MenuItemNavigate("Battery", "Battery");
  MenuItemNavigate systemInfoItem = MenuItemNavigate("System Info", "System Info");
  MenuItemNavigate wifiItem = MenuItemNavigate("Wi-Fi Info", "Wi-Fi info");
  MenuItemNavigate rssiItem = MenuItemNavigate("RSSI", "RSSI");

  MenuItemToggle rainbowModeItem = MenuItemToggle("Rainbow Mode", &rainbowMode, false);

  MenuItemNumber<long> bootCountItem = MenuItemNumber<long>("Boot Count", &bootCount);

  MenuItemAction serialMacAddrItem = MenuItemAction("Serial MAC Address", 2, [&]()
                                                    {
                                                      uint8_t baseMac[6];
                                                      esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
                                                      if (ret == ESP_OK)
                                                      {
                                                        // Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                                                        //               baseMac[0], baseMac[1], baseMac[2],
                                                        //               baseMac[3], baseMac[4], baseMac[5]);
                                                        ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x\n",
                                                                 baseMac[0], baseMac[1], baseMac[2],
                                                                 baseMac[3], baseMac[4], baseMac[5]);
                                                      }
                                                      else
                                                      {
                                                        // Serial.println("Failed to read MAC address");
                                                        ESP_LOGW(TAG, "Failed to read MAC address");
                                                      }
                                                      //
                                                    });

  void draw() override;
  void update() override;
  void onEnter() override;
};

Settings::Settings(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&batteryItem);
  menu.addMenuItem(&systemInfoItem);
  menu.addMenuItem(&wifiItem);
  menu.addMenuItem(&rssiItem);
  menu.addMenuItem(&rainbowModeItem);
  menu.addMenuItem(&bootCountItem);
  menu.addMenuItem(&serialMacAddrItem);

  rainbowModeItem.setOnChange([&]()
                              {
                                btnLed.SetMode(rainbowMode ? RGB_MODE::Manual : RGB_MODE::Rainbow);
                                //
                              });
}

void Settings::onEnter()
{
  bootCount = preferences.getLong("bootCount", 0);
}

void Settings::draw()
{
  menu.draw();
}

void Settings::update()
{
  menu.update();

  rainbowMode = btnLed.GetMode() == RGB_MODE::Rainbow;
}