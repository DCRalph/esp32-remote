#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"

class Settings : public Screen
{
public:
  Settings(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate batteryItem = MenuItemNavigate("Battery", "Battery");
  MenuItemNavigate wifiItem = MenuItemNavigate("Wi-Fi", "Wi-Fi");
  MenuItemNavigate rssiItem = MenuItemNavigate("RSSI", "RSSI");

  void draw() override;
  void update() override;
};

Settings::Settings(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&batteryItem);
  menu.addMenuItem(&wifiItem);
  menu.addMenuItem(&rssiItem);
}

void Settings::draw()
{
  menu.draw();
}

void Settings::update()
{
  menu.update();
}