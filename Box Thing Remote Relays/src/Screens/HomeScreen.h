#pragma once

#include "config.h"
#include "IO/LCD.h"

void homeScreenDraw()
{
  lcd.lcd.setCursor(0, 0);
  lcd.lcd.print("Home");
}

void homeScreenOnEnter()
{
  lcd.lcd.clear();
}

Screen HomeScreen = {
    "Home",
    homeScreenDraw,
    nullptr,
    homeScreenOnEnter,
    nullptr};