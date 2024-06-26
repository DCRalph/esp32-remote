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

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemNavigate ioTestItem = MenuItemNavigate("IO Test", "IO Test");

  MenuItemNumber bootCountItem = MenuItemNumber("Boot", &bootCount);

  void draw() override;
  void update() override;
  void onEnter() override;
};

DebugScreen::DebugScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&ioTestItem);
  menu.addMenuItem(&bootCountItem);
}

void DebugScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Debug");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  menu.draw();
}

void DebugScreen::update()
{
  menu.update();
}

void DebugScreen::onEnter()
{
  bootCount = preferences.getLong("bootCount", 0);
}