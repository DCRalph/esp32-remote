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