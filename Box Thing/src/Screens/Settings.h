#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Screens/Helpers/Menu.h"

class SettingsScreen : public Screen
{
public:
  SettingsScreen(String _name);

  Menu menu = Menu();

  MenuItemAction backItem = MenuItemAction("Back", []()
                                           { screenManager.back(); });

  MenuItemAction generalSettingsItem = MenuItemAction("General", []()
                                                      { screenManager.setScreen("General Settings"); });

  MenuItemAction displaySettingsItem = MenuItemAction("Display", []()
                                                      { screenManager.setScreen("Display Settings"); });

  MenuItemAction wifiItem = MenuItemAction("WiFi", []()
                                           { screenManager.setScreen("WiFi Settings"); });

  MenuItemAction mqttItem = MenuItemAction("MQTT", []()
                                           { screenManager.setScreen("MQTT Info"); });

  MenuItemAction ioTestItem = MenuItemAction("IO Test", [&]()
                                             { screenManager.setScreen("IO Test"); });

  void draw() override;
  void update() override;
  void onEnter() override;
  u8 active;
};

SettingsScreen::SettingsScreen(String _name) : Screen(_name)
{
  active = 1;

  menu.addMenuItem(&backItem);
  menu.addMenuItem(&generalSettingsItem);
  menu.addMenuItem(&displaySettingsItem);
  menu.addMenuItem(&wifiItem);
  menu.addMenuItem(&mqttItem);
  menu.addMenuItem(&ioTestItem);
}

void SettingsScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Settings");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  menu.draw();
}

void SettingsScreen::update()
{
  menu.update();
}

void SettingsScreen::onEnter()
{
  encoder.encoder.setPosition(0);
}