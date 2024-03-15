#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Screens/Helpers/Menu.h"

class GeneralSettingsScreen : public Screen
{
public:
  GeneralSettingsScreen(String _name);

  bool toggle = false;
  int number = 0;

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemToggle toggleItem = MenuItemToggle("Toggle", &toggle);

  MenuItemNumber numberItem = MenuItemNumber("Number", &number, 0, 10);

  void draw() override;
  void update() override;
};

GeneralSettingsScreen::GeneralSettingsScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&toggleItem);
  menu.addMenuItem(&numberItem);
}

void GeneralSettingsScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "General Settings");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  menu.draw();
}

void GeneralSettingsScreen::update()
{
  menu.update();
}