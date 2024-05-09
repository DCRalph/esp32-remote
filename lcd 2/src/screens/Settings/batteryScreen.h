#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include "IO/Battery.h"

class BatteryScreen : public Screen
{
public:
  BatteryScreen(String _name) : Screen(_name){};

  void draw() override;
  void update() override;
};

void BatteryScreen::draw()
{
  display.sprite.setTextDatum(TL_DATUM);

  int percent = battery.getPercentageI();
  percent = constrain(percent, 0, 100);
  int height = map(percent, 0, 100, 0, 121);

  u16_t color = TFT_GREEN;
  if (percent < 20)
    color = TFT_RED;
  else if (percent < 50)
    color = TFT_ORANGE;

  display.sprite.setTextSize(4);
  display.sprite.setTextColor(TFT_GREEN);
  display.sprite.drawString("Battery", 10, 30);

  display.sprite.drawRoundRect((LCD_WIDTH / 4 * 3) - 25, 36, 50, 125, 5, TFT_WHITE);
  display.sprite.fillRect((LCD_WIDTH / 4 * 3) - 5, 29, 10, 7, TFT_WHITE);

  display.sprite.fillRoundRect((LCD_WIDTH / 4 * 3) - 23, 36 + 2 + (121 - height), 46, height, 5, color);

  display.sprite.setTextSize(4);
  display.sprite.setTextColor(TFT_WHITE);

  char buf[10];

  sprintf(buf, "%.2fV", battery.getVoltage());
  display.sprite.drawString(buf, 10, 80);

  sprintf(buf, "%i%%", battery.getPercentageI());
  display.sprite.drawString(buf, 10, 120);
}

void BatteryScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.back();
}