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
  display.sprite.setTextDatum(TL_DATUM);

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

  display.sprite.setTextSize(6);
  display.sprite.setTextColor(color);
  display.sprite.drawString("Battery", 10, 50);

  display.sprite.drawRoundRect(x - (w / 2), y, w, h, cornerRadius, TFT_WHITE); // battery outline
  display.sprite.fillRect(x - (nibW / 2), y - nibH, nibW, nibH, TFT_WHITE);    // battery top

  uint16_t x1 = x - (w / 2) + gap;
  uint16_t y1 = y - gap + (h - height);
  uint16_t w1 = w - gap * 2;
  uint16_t h1 = height;

  display.sprite.fillRoundRect(x1, y1, w1, h1, cornerRadius, color); // battery fill

  display.sprite.setTextSize(4);
  display.sprite.setTextColor(TFT_WHITE);

  char buf[10];

  sprintf(buf, "%.2fV", battery.getVoltage());
  display.sprite.drawString(buf, 10, 120);

  sprintf(buf, "%i%%", battery.getPercentageI());
  display.sprite.drawString(buf, 10, 160);
}

void BatteryScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.back();
}