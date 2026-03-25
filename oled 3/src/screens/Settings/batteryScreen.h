#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include "IO/Battery.h"

class BatteryScreen : public Screen
{
public:
  BatteryScreen(String _name) : Screen(_name) {};

  void draw() override;
  void update() override;
};

void BatteryScreen::draw()
{
  display.setTextDatum(TL_DATUM);

  uint16_t x = 450;
  uint16_t y = 60;
  uint16_t w = 80;
  uint16_t h = LCD_HEIGHT - (y);
  uint8_t gap = 3;
  uint8_t cornerRadius = 10;
  uint8_t nibW = 10;
  uint8_t nibH = 7;

  float percent = battery.getPercentageF();
  percent = constrain(percent, 0, 100);
  int height = map(percent, 0, 100, 0, (h - gap * 2)); // 121 is the height of the battery

  uint16_t color = TFT_GREEN;
  if (percent < 20)
    color = TFT_RED;
  else if (percent < 50)
    color = TFT_ORANGE;

  display.setTextSize(6);
  display.setTextColor(color);
  display.drawString("Battery", 10, 50);

  display.drawRoundRect(x - (w / 2), y, w, h, cornerRadius, TFT_WHITE);
  display.fillRect(x - (nibW / 2), y - nibH, nibW, nibH, TFT_WHITE);

  uint16_t x1 = x - (w / 2) + gap;
  uint16_t y1 = y - gap + (h - height);
  uint16_t w1 = w - gap * 2;
  uint16_t h1 = height;

  display.fillRoundRect(x1, y1, w1, h1, cornerRadius, color);

  display.setTextSize(4);
  display.setTextColor(TFT_WHITE);

  char buf[10];

  sprintf(buf, "%.2fV", battery.getVoltage());
  display.drawString(buf, 10, 120);

  sprintf(buf, "%i%%", battery.getPercentageI());
  display.drawString(buf, 10, 160);
}

void BatteryScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.back();
}