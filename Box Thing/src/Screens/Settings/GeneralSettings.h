#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Screens/Helpers/Menu.h"

class GeneralSettingsScreen : public Screen
{
public:
  GeneralSettingsScreen(String _name);

  Menu menu = Menu();

  MenuItem backItem = MenuItem("Back", []()
                               { screenManager.back(); });

  void draw() override;
  void update() override;
};

GeneralSettingsScreen::GeneralSettingsScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
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