#pragma once

#include "config.h"
#include "IO/LCD.h"

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