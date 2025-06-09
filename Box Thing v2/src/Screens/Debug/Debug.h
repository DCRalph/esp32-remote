#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "Setup/InitOTA.h"

#include "IO/Menu.h"

static String formatBytes(size_t bytes, bool _short = false)
{
  if (bytes < 1024)
  {
    if (_short)
    {
      // Since bytes is an integer, leave it as is.
      return String(bytes);
    }
    else
    {
      return String(bytes) + " B";
    }
  }
  else if (bytes < (1024UL * 1024UL))
  {
    // Force floating point math by casting bytes to double.
    double kb = ((double)bytes) / 1024.0;
    if (_short)
    {
      return String(kb, 1) + "K";
    }
    else
    {
      return String(kb, 2) + " KB";
    }
  }
  else
  {
    double mb = ((double)bytes) / (1024.0 * 1024.0);
    if (_short)
    {
      return String(mb, 1) + "M";
    }
    else
    {
      return String(mb, 2) + " MB";
    }
  }
}

class DebugScreen : public Screen
{
public:
  DebugScreen(String _name);

  long bootCount;
  bool ledState = false;
  bool otaSetupTmp = false;

  uint64_t totalMem = 0;
  uint64_t freeMem = 0;
  uint64_t usedMem = 0;

  String totalMemStr = "0";
  String freeMemStr = "0";
  String usedMemStr = "0";

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate ioTestItem = MenuItemNavigate("IO Test", "IO Test");

  MenuItemNavigate batteryItem = MenuItemNavigate("Battery", "Battery");

  MenuItemToggle ledItem = MenuItemToggle("LED", &ledState);

  MenuItemNumber<long> bootCountItem = MenuItemNumber<long>("Boot", &bootCount);

  MenuItemToggle otaItem = MenuItemToggle("OTA", &otaSetupTmp);

  MenuItemNumber<uint32_t> fpsItem = MenuItemNumber<uint32_t>("FPS", &lastFps);
  MenuItemNumber<uint32_t> frameTimeItem = MenuItemNumber<uint32_t>("F Time", &lastFrameTime);

  MenuItemNumber<uint64_t> clearBufferTimeItem = MenuItemNumber<uint64_t>("clrbuf", &clearBufferTime);
  MenuItemNumber<uint64_t> screenManagerDrawTimeItem = MenuItemNumber<uint64_t>("smd", &screenManagerDrawTime);
  MenuItemNumber<uint64_t> drawTopBarTimeItem = MenuItemNumber<uint64_t>("dtb", &drawTopBarTime);
  MenuItemNumber<uint64_t> sendBufferTimeItem = MenuItemNumber<uint64_t>("sb", &sendBufferTime);
  MenuItemNumber<uint64_t> screenUpdateDrawTimeItem = MenuItemNumber<uint64_t>("sud", &screenUpdateDrawTime);

  MenuItemString totalMemItem = MenuItemString("tmem", &totalMemStr);
  MenuItemString freeMemItem = MenuItemString("fmem", &freeMemStr);
  MenuItemString usedMemItem = MenuItemString("umem", &usedMemStr);

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

  menu.addMenuItem(&totalMemItem);
  menu.addMenuItem(&freeMemItem);
  menu.addMenuItem(&usedMemItem);

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

  totalMem = ESP.getHeapSize();
  freeMem = ESP.getFreeHeap();
  usedMem = totalMem - freeMem;

  totalMemStr = formatBytes(totalMem, true);
  freeMemStr = formatBytes(freeMem, true);
  usedMemStr = formatBytes(usedMem, true);
}

void DebugScreen::onEnter()
{

  bootCount = preferences.getLong("bootCount", 0);
  ledState = led.read();

  otaSetupTmp = otaSetup;
}