#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "IO/Wireless.h"

#include "IO/Menu.h"

class WiFiSettingsScreen : public Screen
{
public:
  WiFiSettingsScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate wifiInfoItem = MenuItemNavigate("WiFi Info", "WiFi Info");

  bool wifiActive = false;
  bool configPortalActive = false;
  bool espnowActive = false;

  MenuItemToggle wifiItem = MenuItemToggle("WiFi", &wifiActive);

  MenuItemToggle toggleESPNOWItem = MenuItemToggle("ESPNOW", &espnowActive);

  MenuItemToggle configPortalItem = MenuItemToggle("Config", &configPortalActive);

  MenuItemAction wifiForgetItem = MenuItemAction("Reset WiFi", 1, [&]()
                                                 {
                                                   wm.resetSettings();
                                                   //  updateButtons();
                                                   //
                                                 });

  void draw() override;
  void update() override;
  void onEnter() override;
  void updateButtons();

  uint8_t active;
};

WiFiSettingsScreen::WiFiSettingsScreen(String _name) : Screen(_name)
{
  active = 1;

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&wifiInfoItem);
  menu.addMenuItem(&wifiItem);
  menu.addMenuItem(&toggleESPNOWItem);
  menu.addMenuItem(&configPortalItem);
  menu.addMenuItem(&wifiForgetItem);

  wifiItem.setOnChange([&]()
                       {
                         if (WiFi.status() == WL_CONNECTED)
                           wm.disconnect();
                         else
                         {
                           if (wm.getConfigPortalActive() || wm.getWebPortalActive()) // if config portal is on
                           {
                             wm.stopConfigPortal();
                             wm.stopWebPortal();
                           }

                           if (wireless.isSetupDone()) // espnow is on
                           {
                             wireless.unSetup();
                             preferences.putBool("espnowOn", false);
                           }

                           wm.autoConnect(AP_SSID);
                         }

                         //  updateButtons();
                         //
                       });

  toggleESPNOWItem.setOnChange([&]()
                               {
                                 if (wireless.isSetupDone()) // espnow is on
                                 {
                                   wireless.unSetup();
                                   preferences.putBool("espnowOn", false);
                                 }
                                 else
                                 {
                                   if (wm.getConfigPortalActive() || wm.getWebPortalActive()) // if config portal is on
                                   {
                                     wm.stopConfigPortal();
                                     wm.stopWebPortal();
                                   }

                                   wm.disconnect();
                                   wireless.setup();
                                   preferences.putBool("espnowOn", true);
                                 }

                                 //  updateButtons();
                                 //
                               });

  configPortalItem.setOnChange([&]()
                               {
                                 if (wm.getConfigPortalActive() || wm.getWebPortalActive())
                                 {
                                   wm.stopConfigPortal();
                                   wm.stopWebPortal();
                                 }
                                 else
                                 {
                                   if (wireless.isSetupDone()) // espnow is on
                                   {
                                     wireless.unSetup();
                                   }
                                   wm.disconnect();
                                   wm.startConfigPortal(AP_SSID);
                                 }
                                 //  updateButtons();
                                 //
                               });

  // updateButtons();
}

void WiFiSettingsScreen::draw()
{
  menu.draw();
}

void WiFiSettingsScreen::update()
{
  menu.update();
  updateButtons();
}

void WiFiSettingsScreen::onEnter()
{
  // updateButtons();
}

void WiFiSettingsScreen::updateButtons()
{
  wifiItem.set(WiFi.status() == WL_CONNECTED);
  toggleESPNOWItem.set(wireless.isSetupDone());
  configPortalItem.set(wm.getConfigPortalActive() || wm.getWebPortalActive());
}