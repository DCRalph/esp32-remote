#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "Setup/InitOTA.h"

#include "IO/Menu.h"

class DebugScreen : public Screen
{
public:
  DebugScreen(String _name);

  long bootCount;
  bool ledState = false;
  bool otaSetupTmp = false;
  uint8_t tileHeight = 0;

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate ioTestItem = MenuItemNavigate("IO Test", "IO Test");

  MenuItemNavigate batteryItem = MenuItemNavigate("Battery", "Battery");

  MenuItemToggle ledItem = MenuItemToggle("LED", &ledState);

  MenuItemNumber<long> bootCountItem = MenuItemNumber<long>("Boot", &bootCount);

  MenuItemToggle otaItem = MenuItemToggle("OTA", &otaSetupTmp);

  MenuItemNumber<uint32_t> fpsItem = MenuItemNumber<uint32_t>("FPS", &lastFps);
  MenuItemNumber<uint32_t> frameTimeItem = MenuItemNumber<uint32_t>("F Time", &lastFrameTime);

  MenuItemNumber<uint64_t> clearBufferTimeItem = MenuItemNumber<uint64_t>("cb", &clearBufferTime);
  MenuItemNumber<uint64_t> screenManagerDrawTimeItem = MenuItemNumber<uint64_t>("smd", &screenManagerDrawTime);
  MenuItemNumber<uint64_t> drawTopBarTimeItem = MenuItemNumber<uint64_t>("dtb", &drawTopBarTime);
  MenuItemNumber<uint64_t> sendBufferTimeItem = MenuItemNumber<uint64_t>("sb", &sendBufferTime);
  MenuItemNumber<uint64_t> screenUpdateDrawTimeItem = MenuItemNumber<uint64_t>("sud", &screenUpdateDrawTime);

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

  menu.addMenuItem(&fpsItem);
  menu.addMenuItem(&frameTimeItem);

  menu.addMenuItem(&clearBufferTimeItem);
  menu.addMenuItem(&screenManagerDrawTimeItem);
  menu.addMenuItem(&drawTopBarTimeItem);
  menu.addMenuItem(&sendBufferTimeItem);
  menu.addMenuItem(&screenUpdateDrawTimeItem);

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