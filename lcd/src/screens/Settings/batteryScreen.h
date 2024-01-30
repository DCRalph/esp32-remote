#pragma once

#include <Arduino.h>
#include "driver/Display.h"
#include "Buttons.h"
#include "Battery.h"

class batteryScreen : public Screen
{
public:
  batteryScreen(String _name, String _id) : Screen(_name, _id){};

  void draw() override;
  void update() override;
};

void batteryScreen::draw()
{
  display.sprite.setTextDatum(TL_DATUM);

  display.sprite.setTextSize(3);
  display.sprite.setTextColor(TFT_CYAN);
  display.sprite.drawString("Battery", 10, 30);

  display.sprite.drawRoundRect((LCD_WIDTH / 4 * 3) - 25, 36, 50, 125, 5, TFT_WHITE);
  display.sprite.fillRect((LCD_WIDTH / 4 * 3) - 5, 29, 10, 7, TFT_WHITE);

  int percent = battery.getPercentageI();
  int height = map(percent, 0, 100, 0, 121);

  display.sprite.fillRoundRect((LCD_WIDTH / 4 * 3) - 23, 36 + 2 + (121 - height), 46, height, 5, TFT_WHITE);

  display.sprite.setTextSize(4);
  display.sprite.setTextColor(TFT_WHITE);

  char buf[10];

  sprintf(buf, "%.2fV", battery.getVoltage());
  display.sprite.drawString(buf, 10, 80);

  sprintf(buf, "%i%%", battery.getPercentageI());
  display.sprite.drawString(buf, 10, 120);
}

void batteryScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.setScreen("settings");
}