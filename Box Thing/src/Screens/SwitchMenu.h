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

  MenuItemAction backItem = MenuItemAction("Back", []()
                                           { screenManager.back(); });

  MenuItemAction mqttSwitchItem = MenuItemAction("MQTT SW", []()
                                                 { screenManager.setScreen("MQTT Switch"); });

  void draw() override;
  void update() override;
};

SwitchMenuScreen::SwitchMenuScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&mqttSwitchItem);
}

void SwitchMenuScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Switch");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  menu.draw();
}

void SwitchMenuScreen::update()
{
  menu.update();
}