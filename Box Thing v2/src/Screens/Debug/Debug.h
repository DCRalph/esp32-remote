#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Screens/Helpers/Menu.h"

class DebugScreen : public Screen
{
public:
  DebugScreen(String _name);

  long bootCount;
  bool ledState = false;

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate ioTestItem = MenuItemNavigate("IO Test", "IO Test");

  MenuItemNavigate batteryItem = MenuItemNavigate("Battery", "Battery");

  MenuItemNumber<long> bootCountItem = MenuItemNumber<long>("Boot", &bootCount);

  MenuItemToggle ledItem = MenuItemToggle("LED", &ledState);

  void draw() override;
  void update() override;
  void onEnter() override;
};

DebugScreen::DebugScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&ioTestItem);
  menu.addMenuItem(&batteryItem);
  menu.addMenuItem(&ledItem);
  menu.addMenuItem(&bootCountItem);

  ledItem.setOnChange([&]()
                      { led.Write(ledState); });
}

void DebugScreen::draw()
{
  menu.draw();
}

void DebugScreen::update()
{
  menu.update();
}

void DebugScreen::onEnter()
{
  bootCount = preferences.getLong("bootCount", 0);
  ledState = led.read();
}