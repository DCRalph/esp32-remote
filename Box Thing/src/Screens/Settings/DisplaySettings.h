#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Screens/Helpers/Menu.h"

class DisplaySettingsScreen : public Screen
{
public:
  DisplaySettingsScreen(String _name);

  Menu menu = Menu();

  MenuItem backItem = MenuItem("Back", []()
                               { screenManager.back(); });

  void draw() override;
  void update() override;
};

DisplaySettingsScreen::DisplaySettingsScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
}

void DisplaySettingsScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Display Settings");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  menu.draw();
}

void DisplaySettingsScreen::update()
{
  menu.update();
}