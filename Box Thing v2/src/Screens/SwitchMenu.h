#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "IO/Menu.h"

class SwitchMenuScreen : public Screen
{
public:
  SwitchMenuScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  // MenuItemNavigate espnowSwitchItem = MenuItemNavigate("ESP SW", "Espnow Switch");

  // MenuItemNavigate carItem = MenuItemNavigate("Car", "Car Control");

  MenuItemNavigate remoteRelayItem = MenuItemNavigate("Relay", "Remote Relay");

  MenuItemNavigate encoderTransmiterItem = MenuItemNavigate("Encoder", "Encoder Transmiter");

  MenuItemNavigate servosControlItem = MenuItemNavigate("Servos", "Servos Control");

  void draw() override;
  void update() override;
};

SwitchMenuScreen::SwitchMenuScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  // menu.addMenuItem(&espnowSwitchItem);
  // menu.addMenuItem(&carItem);
  menu.addMenuItem(&remoteRelayItem);
  menu.addMenuItem(&encoderTransmiterItem);
  menu.addMenuItem(&servosControlItem);
}

void SwitchMenuScreen::draw()
{
  menu.draw();
}

void SwitchMenuScreen::update()
{
  menu.update();
}