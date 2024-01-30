#pragma once

#include <Arduino.h>
#include "driver/Display.h"
#include "Buttons.h"
#include "Battery.h"

class wifiScreen : public Screen
{
public:
  wifiScreen(String _name, String _id) : Screen(_name, _id){};

  void draw() override;
  void update() override;
};

void wifiScreen::draw()
{
  display.sprite.setTextDatum(TL_DATUM);

  display.sprite.setTextSize(3);
  display.sprite.setTextColor(TFT_CYAN);
  display.sprite.drawString("Wi-Fi", 10, 30);

 
}

void wifiScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.setScreen("settings");
}