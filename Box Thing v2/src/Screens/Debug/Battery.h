#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "IO/Battery.h"

class BatteryScreen : public Screen
{
public:
  BatteryScreen(String _name);

  void draw() override;
  void update() override;
};

BatteryScreen::BatteryScreen(String _name) : Screen(_name)
{
}

void BatteryScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "Battery");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  display.u8g2.setFont(u8g2_font_logisoso16_tf);
  display.u8g2.setDrawColor(1);

  char buffer[32];
  sprintf(buffer, "%.2fV. %d%", batteryGetVoltage(), batteryGetPercentage());
  display.u8g2.drawStr(0, 28, buffer);

}

void BatteryScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }
}