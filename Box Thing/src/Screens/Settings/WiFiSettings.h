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

  MenuItemAction configPortalItem = MenuItemAction("Config ...", 1, [&]()
                                                   {
                                                     if (wm.getConfigPortalActive() || wm.getWebPortalActive())
                                                     {
                                                       wm.stopConfigPortal();
                                                       wm.stopWebPortal();

                                                       wm.autoConnect(AP_SSID);

                                                       configPortalItem.setName("Config ON");
                                                     }
                                                     else
                                                     {
                                                       wm.disconnect();
                                                       wm.startConfigPortal(AP_SSID);

                                                       configPortalItem.setName("Config OFF");
                                                     }
                                                     updateButtons();
                                                     //
                                                   });

  MenuItemAction toggleESPNOWItem = MenuItemAction("ESPNOW ...", 1, [&]()
                                                   {
                                                     if (wireless.isSetupDone())
                                                     {
                                                       wireless.unSetup();
                                                       wm.autoConnect(AP_SSID);
                                                       toggleESPNOWItem.setName("ESPNOW ON");
                                                     }
                                                     else
                                                     {
                                                       wm.disconnect();
                                                       wireless.setup();
                                                       toggleESPNOWItem.setName("ESPNOW OFF");
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
  menu.addMenuItem(&toggleESPNOWItem);
  menu.addMenuItem(&wifiForgetItem);

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
    configPortalItem.setName("Config ON");
  else
    configPortalItem.setName("Config OFF");

  if (wireless.isSetupDone())
    toggleESPNOWItem.setName("ESPNOW ON");
  else
    toggleESPNOWItem.setName("ESPNOW OFF");
}