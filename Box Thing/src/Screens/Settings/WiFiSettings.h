#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Screens/Helpers/Menu.h"

class WiFiSettingsScreen : public Screen
{
public:
  WiFiSettingsScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate wifiInfoItem = MenuItemNavigate("WiFi Info", "WiFi Info");

  MenuItemAction configPortalItem = MenuItemAction("Start Conf", [&]()
                                       {
                                         if (wm.getConfigPortalActive() || wm.getWebPortalActive())
                                         {
                                            wm.stopConfigPortal();
                                            wm.stopWebPortal();

                                            wm.autoConnect(AP_SSID);

                                            configPortalItem.setName("Start Conf");
                                         }
                                         else
                                         {
                                            wm.disconnect();
                                            wm.startConfigPortal(AP_SSID);

                                            configPortalItem.setName("Stop Conf");
                                         } });

  MenuItemAction wifiForgetItem = MenuItemAction("Reset WiFi", []()
                                     { wm.resetSettings(); });

  void draw() override;
  void update() override;
  void onEnter() override;
  u8 active;
};

WiFiSettingsScreen::WiFiSettingsScreen(String _name) : Screen(_name)
{
  active = 1;

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&wifiInfoItem);
  menu.addMenuItem(&configPortalItem);
  menu.addMenuItem(&wifiForgetItem);

  if (wm.getConfigPortalActive() || wm.getWebPortalActive())
    configPortalItem.setName("Stop Conf");
  else
    configPortalItem.setName("Start Conf");
}

void WiFiSettingsScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "WiFi Settings");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  menu.draw();
}

void WiFiSettingsScreen::update()
{
  menu.update();
}

void WiFiSettingsScreen::onEnter()
{
  encoder.encoder.setPosition(0);
}