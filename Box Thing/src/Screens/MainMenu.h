#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

#include "Helpers/Menu.h"

class MainMenuScreen : public Screen
{
public:
  MainMenuScreen(String _name);

  Menu mainMenu = Menu("Main Menu Menu");

  MenuItem homeItem = MenuItem("Home", []()
                               { screenManager.setScreen("Home"); });

  MenuItem settingsItem = MenuItem("Settings", []()
                                   { screenManager.setScreen("Settings"); });

  MenuItem wifiItem = MenuItem("WiFi", []()
                               { screenManager.setScreen("WiFi Info"); });

  MenuItem testItem = MenuItem("Test", [&]()
                               { testItem.setName("Tested"); });

  MenuItem resetTestItem = MenuItem("Reset test", [&]()
                                    { testItem.setName("Test"); });

  void draw() override;
  void update() override;
  void onEnter() override;
  u8 active;
};

MainMenuScreen::MainMenuScreen(String _name) : Screen(_name)
{
  active = 1;

  mainMenu.addMenuItem(&homeItem);
  mainMenu.addMenuItem(&settingsItem);
  mainMenu.addMenuItem(&wifiItem);
  mainMenu.addMenuItem(&testItem);
  mainMenu.addMenuItem(&resetTestItem);

  testItem.setName("Test");

  // testItem.setFunction([&]()
  //                      { testItem.setName("Tested"); });
}

void MainMenuScreen::draw()
{
  display.u8g2.setFont(u8g2_font_t0_11b_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Main Menu");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  mainMenu.draw();
}

void MainMenuScreen::update()
{
  mainMenu.update();
}

void MainMenuScreen::onEnter()
{
  encoder.encoder.setPosition(0);
}