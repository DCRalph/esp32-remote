#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Screens/Helpers/Menu.h"

class SettingsScreen : public Screen
{
public:
  SettingsScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate generalSettingsItem = MenuItemNavigate("General", "General Settings");

  MenuItemNavigate displaySettingsItem = MenuItemNavigate("Display", "Display Settings");

  MenuItemNavigate wifiItem = MenuItemNavigate("WiFi", "WiFi Settings");

  MenuItemNavigate mqttItem = MenuItemNavigate("MQTT", "MQTT Info");

  void draw() override;
  void update() override;
  void onEnter() override;
  uint8_t active;
};

SettingsScreen::SettingsScreen(String _name) : Screen(_name)
{
  active = 1;

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&generalSettingsItem);
  menu.addMenuItem(&displaySettingsItem);
  menu.addMenuItem(&wifiItem);
  menu.addMenuItem(&mqttItem);
}

void SettingsScreen::draw()
{
  menu.draw();
}

void SettingsScreen::update()
{
  menu.update();
}

void SettingsScreen::onEnter()
{
  encoder.setPosition(0);
}