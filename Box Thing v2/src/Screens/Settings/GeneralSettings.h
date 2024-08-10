#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include <WiFi.h>

#include "Screens/Helpers/Menu.h"

class GeneralSettingsScreen : public Screen
{
public:
  GeneralSettingsScreen(String _name);

  unsigned long lastUpdate = 0;
  long ramPercentage = 0;

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNumber<long> ramUsageItem = MenuItemNumber<long>("Ram", &ramPercentage);

  MenuItemNumber<int> autoOffItem = MenuItemNumber<int>("Auto Off", &autoOffMin, 0, 60);

  void draw() override;
  void update() override;
  void onEnter() override;
  void onExit() override;
};

GeneralSettingsScreen::GeneralSettingsScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&ramUsageItem);
  menu.addMenuItem(&autoOffItem);

  autoOffItem.setOnChange([this]()
                          { preferences.putInt("autoOffMin", autoOffMin);
                          Serial.println("Auto Off Time: " + String(autoOffMin)); });
}

void GeneralSettingsScreen::draw()
{
  menu.draw();
}

void GeneralSettingsScreen::update()
{
  if (millis() - lastUpdate > 1000)
  {
    lastUpdate = millis();
    ramPercentage = (int)(ESP.getFreeHeap() * 100 / ESP.getHeapSize());
  }

  menu.update();
}

void GeneralSettingsScreen::onEnter()
{
}

void GeneralSettingsScreen::onExit()
{
}
