#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"

class Settings : public Screen
{
public:
  Settings(String _name);

  long bootCount;

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate batteryItem = MenuItemNavigate("Battery", "Battery");
  MenuItemNavigate wifiItem = MenuItemNavigate("Wi-Fi Info", "Wi-Fi info");
  MenuItemNavigate rssiItem = MenuItemNavigate("RSSI", "RSSI");
  MenuItemNumber bootCountItem = MenuItemNumber("Boot Count", &bootCount);

  MenuItemAction serialMacAddrItem = MenuItemAction("Serial MAC Address", 2, [&]()
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
  menu.addMenuItem(&wifiItem);
  menu.addMenuItem(&rssiItem);
  menu.addMenuItem(&bootCountItem);
  menu.addMenuItem(&serialMacAddrItem);
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
}