#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Helpers/Menu.h"

class HomeScreen : public Screen
{
public:
  HomeScreen(String _name);

  Menu menu = Menu();

  MenuItem settingsItem = MenuItem("Settings", []()
                                   { screenManager.setScreen("Settings"); });

  MenuItem powerOffItem = MenuItem("Power Off", []()
                                   { latch.Off(); });

  void draw() override;
  void update() override;
  void onEnter() override;
  u8 active;
};

HomeScreen::HomeScreen(String _name) : Screen(_name)
{
  active = 1;

  menu.addMenuItem(&settingsItem);
  menu.addMenuItem(&powerOffItem);

  powerOffItem.setClicksToRun(-1);
}

void HomeScreen::draw()
{
  display.u8g2.setFont(u8g2_font_t0_11b_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Home");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  menu.draw();
}

void HomeScreen::update()
{
  menu.update();
}

void HomeScreen::onEnter()
{
  encoder.encoder.setPosition(0);
}