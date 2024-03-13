#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Helpers/Menu.h"

class WiFiSettingsScreen : public Screen
{
public:
  WiFiSettingsScreen(String _name);

  Menu menu = Menu();

  MenuItem backItem = MenuItem("Back", []()
                               { screenManager.back(); });

  MenuItem wifiInfoItem = MenuItem("WiFi Info", []()
                                   { screenManager.setScreen("WiFi Info"); });

  MenuItem configPortalItem = MenuItem("Start Config", [&]()
                                       {
                                         if (wm.getConfigPortalActive() || wm.getWebPortalActive())
                                         {
                                            wm.stopConfigPortal();
                                            wm.stopWebPortal();

                                            wm.autoConnect(AP_SSID);

                                            configPortalItem.setName("Start Config");
                                         }
                                         else
                                         {
                                            wm.disconnect();
                                            wm.startConfigPortal(AP_SSID);

                                            configPortalItem.setName("Stop Config");
                                         } });

  MenuItem wifiForgetItem = MenuItem("Forget WiFi", []()
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
    configPortalItem.setName("Stop Config");
  else
    configPortalItem.setName("Start Config");
}

void WiFiSettingsScreen::draw()
{
  display.u8g2.setFont(u8g2_font_t0_11b_tf);
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