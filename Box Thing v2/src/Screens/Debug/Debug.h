#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "IO/Menu.h"

class DebugScreen : public Screen
{
public:
  DebugScreen(String _name);

  long bootCount;
  bool ledState = false;
  bool otaSetupTmp = false;

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate ioTestItem = MenuItemNavigate("IO Test", "IO Test");

  MenuItemNavigate batteryItem = MenuItemNavigate("Battery", "Battery");

  MenuItemToggle ledItem = MenuItemToggle("LED", &ledState);

  MenuItemNumber<long> bootCountItem = MenuItemNumber<long>("Boot", &bootCount);

  MenuItemToggle otaItem = MenuItemToggle("OTA", &otaSetupTmp);

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
  menu.addMenuItem(&otaItem);

  ledItem.setOnChange([&]()
                      { led.Write(ledState); });

  otaItem.setOnChange([&]()
                      {
    if (!otaSetup){
      InitOta();
    }
    otaSetupTmp = true; });
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

  otaSetupTmp = otaSetup;
}