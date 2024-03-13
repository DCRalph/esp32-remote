#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

class UpdateProgressScreen : public Screen
{
public:
  UpdateProgressScreen(String _name);

  void draw() override;

  u8 progress;
  u8 state;

  String msg;

  void setProgress(u8 _progress);
  void setMessage(String _msg);
};

UpdateProgressScreen::UpdateProgressScreen(String _name) : Screen(_name)
{
  progress = 0;
  state = 0;
}

void UpdateProgressScreen::draw()
{
  display.u8g2.setFont(u8g2_font_logisoso24_tr);
  display.drawCenteredText(24, "Updating");

  switch (state)
  {
  case 0: // Getting ready
    display.u8g2.setFont(u8g2_font_logisoso16_tr);
    display.drawCenteredText(64, "Getting ready...");
    break;

  case 1: // Updating
    display.u8g2.drawFrame(5, 32, 123, 16);
    display.u8g2.drawBox(7, 34, map(progress, 0, 100, 0, 119), 12);
    break;

  case 2: // Showing message
    display.u8g2.setFont(u8g2_font_logisoso16_tr);
    display.drawCenteredText(50, msg);
    break;

  default:
    break;
  }
}

void UpdateProgressScreen::setProgress(u8 _progress)
{
  state = 1;
  progress = _progress;
}

void UpdateProgressScreen::setMessage(String _msg)
{
  state = 2;
  msg = _msg;
}
