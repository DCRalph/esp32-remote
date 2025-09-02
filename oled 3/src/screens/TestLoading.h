#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"

class TestLoadingScreen : public Screen
{
public:
  TestLoadingScreen(String _name) : Screen(_name) {}

  void draw() override
  {
    // Draw a spinner using an arc that completes one full rotation per second
    // Angle 0 is at 6 o'clock and increases clockwise in TFT_eSPI
    const int cx = LCD_WIDTH / 2;
    const int cy = LCD_HEIGHT / 2 + 10;
    const int outerR = 40;
    const int innerR = 26; // thickness ~14px

    // Compute angle from millis so 360 deg per 1000 ms
    unsigned long now = millis();
    int base = (now % 1000UL) * 360 / 1000UL; // 0..359

    // Spinner segment sweep
    const int sweep = 90; // 90-degree visible segment

    // Foreground spinning arc (anti-aliased)
    int startA = base % 360;
    int endA = (base + sweep) % 360; // ensure proper wrap across 360
    display.sprite.drawSmoothArc(cx, cy, outerR, innerR, startA, endA, TFT_CYAN, TFT_BLACK, true);

    // Optional center text
    display.sprite.setTextDatum(TC_DATUM);
    display.sprite.setTextSize(2);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString("Loading...", LCD_WIDTH / 2, cy + outerR + 20);
  }

  void update() override
  {
    // Back with any button
    if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
      screenManager.back();
  }

  void onEnter() override
  {
    topBarColor = TFT_DARKGREY;
    topBarTextColor = TFT_WHITE;
  }
};
