#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "IO/Wireless.h"

#include "Screens/Helpers/Menu.h"

class WiFiSettingsScreen : public Screen
{
public:
  WiFiSettingsScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate wifiInfoItem = MenuItemNavigate("WiFi Info", "WiFi Info");

  MenuItemAction configPortalItem = MenuItemAction("Start Conf", 1, [&]()
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
                                                     }
                                                     updateButtons();
                                                     //
                                                   });

  MenuItemAction wifiForgetItem = MenuItemAction("Reset WiFi", 1, [&]()
                                                 {
                                                   wm.resetSettings();
                                                   updateButtons();
                                                   //
                                                 });

  MenuItemAction toggleESPNOWItem = MenuItemAction("ESPNOW ON", 1, [&]()
                                                   {
                                                     if (wireless.isSetupDone())
                                                     {
                                                       wireless.unsetup();
                                                       wm.autoConnect(AP_SSID);
                                                       toggleESPNOWItem.setName("ESPNOW OFF");
                                                     }
                                                     else
                                                     {
                                                       wm.disconnect();
                                                       wireless.setup();
                                                       toggleESPNOWItem.setName("ESPNOW ON");
                                                     }
                                                     updateButtons();
                                                     //
                                                   });

  void draw() override;
  void update() override;
  void onEnter() override;
  void updateButtons();

  u8 active;
};

WiFiSettingsScreen::WiFiSettingsScreen(String _name) : Screen(_name)
{
  active = 1;

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&wifiInfoItem);
  menu.addMenuItem(&configPortalItem);
  menu.addMenuItem(&wifiForgetItem);
  menu.addMenuItem(&toggleESPNOWItem);

  updateButtons();
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

void WiFiSettingsScreen::updateButtons()
{
  if (wm.getConfigPortalActive() || wm.getWebPortalActive())
    configPortalItem.setName("Stop Conf");
  else
    configPortalItem.setName("Start Conf");

  if (wireless.isSetupDone())
    toggleESPNOWItem.setName("ESPNOW OFF");
  else
    toggleESPNOWItem.setName("ESPNOW ON");
}