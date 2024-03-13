#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Helpers/Menu.h"

class SettingsScreen : public Screen
{
public:
  SettingsScreen(String _name);

  Menu menu = Menu();

  MenuItem backItem = MenuItem("Back", []()
                               { screenManager.back(); });

  MenuItem wifiItem = MenuItem("WiFi", []()
                               { screenManager.setScreen("WiFi Settings"); });


  MenuItem ioTestItem = MenuItem("IO Test", [&]()
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
  menu.addMenuItem(&wifiItem);
  menu.addMenuItem(&ioTestItem);
}

void SettingsScreen::draw()
{
  display.u8g2.setFont(u8g2_font_t0_11b_tf);
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