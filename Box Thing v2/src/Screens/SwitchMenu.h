#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "Screens/Helpers/Menu.h"

class SwitchMenuScreen : public Screen
{
public:
  SwitchMenuScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate mqttSwitchItem = MenuItemNavigate("MQTT SW", "MQTT Switch");
  MenuItemNavigate espnowSwitchItem = MenuItemNavigate("ESP SW", "Espnow Switch");

  void draw() override;
  void update() override;
};

SwitchMenuScreen::SwitchMenuScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&mqttSwitchItem);
  menu.addMenuItem(&espnowSwitchItem);
}

void SwitchMenuScreen::draw()
{
  menu.draw();
}

void SwitchMenuScreen::update()
{
  menu.update();
}