#pragma once

#include "config.h"
#include "IO/LCD.h"
#include "IO/Battery.h"

void homeScreenDraw()
{
  lcd.lcd.setCursor(0, 0);
  lcd.lcd.print("Home");

  lcd.lcd.setCursor(6, 0);

  switch (armed.get())
  {
  case ArmedState::LOCKED:
    lcd.lcd.print("  Lock");
    break;
  case ArmedState::DISARMED:
    lcd.lcd.print("Disarm");
    break;
  case ArmedState::READY:
    lcd.lcd.print(" Ready");
    break;
  case ArmedState::ARMED:
    lcd.lcd.print(" Armed");
    break;
  default:
    break;
  }

  lcd.lcd.setCursor(13, 0);
  lcd.lcd.print("Remote");

  if (remoteConnected)
  {
    lcd.lcd.print(char(LCD_CHECK));
  }
  else
  {
    lcd.lcd.print(char(LCD_CROSS));
  }

  lcd.lcd.setCursor(0, 1);
  lcd.lcd.print("Battery Info:");

  char buffer[32];
  sprintf(buffer, "1: %d%%   2: %d%%  ", batteryGetPercentageSmooth(), battery2GetPercentageSmooth());

  lcd.lcd.setCursor(0, 2);
  lcd.lcd.print(buffer);

  sprintf(buffer, "1: %.2fV 2: %.2fV  ", batteryGetVoltageSmooth(), battery2GetVoltageSmooth());

  lcd.lcd.setCursor(0, 3);
  lcd.lcd.print(buffer);
}

void homeScreenOnEnter()
{
  lcd.lcd.clear();
}

Screen HomeScreen("Home", homeScreenDraw, nullptr, homeScreenOnEnter, nullptr);

/**
 * Home    Arm0 Remote0
 *
 */