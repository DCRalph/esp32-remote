#pragma once

#include "config.h"
#include "display/Display.h"
#include "IO/Buttons.h"
#include "IO/Wireless.h"

class CarLocksScreen : public Screen
{
public:
  CarLocksScreen(String _name);

  void draw() override;
  void update() override;
};

CarLocksScreen::CarLocksScreen(String _name) : Screen(_name)
{
}

void CarLocksScreen::draw()
{
  display.noTopBar();

  display.setTextDatum(TL_DATUM);
  display.setTextColor(TFT_WHITE);
  display.setTextSize(4);
  
  display.setTextDatum(TR_DATUM);
  display.setTextColor(TFT_WHITE);
  display.setTextSize(3);
  display.drawString("Lock ->", LCD_WIDTH - 2, 2);

  display.setTextDatum(BR_DATUM);
  display.setTextColor(TFT_WHITE);
  display.setTextSize(3);
  display.drawString("(exit) Unlock ->", LCD_WIDTH - 2, LCD_HEIGHT - 2);
}

void CarLocksScreen::update()
{

  if (ClickButtonUP.clicks == 1)
  {
    data_packet p;
    p.type = 0x10; // car locks
    p.len = 1;
    p.data[0] = 0; // lock

    wireless.send(&p, car_addr);
  }

  if (ClickButtonDOWN.clicks == -1)
  {
    data_packet p;
    p.type = 0x10; // car locks
    p.len = 1;
    p.data[0] = 1; // unlock

    wireless.send(&p, car_addr);
  }

  if (ClickButtonDOWN.clicks == 1)
    screenManager.back();
}