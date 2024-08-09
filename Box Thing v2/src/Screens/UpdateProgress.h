#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

enum UpdateProgressState
{
  GETTING_READY,
  UPDATING,
  SHOWING_MESSAGE
};

class UpdateProgressScreen : public Screen
{
public:
  UpdateProgressScreen(String _name);

  void draw() override;

  uint8_t progress;
  UpdateProgressState state;

  String msg;

  void setProgress(uint8_t _progress);
  void setMessage(String _msg);
};

UpdateProgressScreen::UpdateProgressScreen(String _name) : Screen(_name)
{
  progress = 0;
  state = UpdateProgressState::GETTING_READY;
}

void UpdateProgressScreen::draw()
{
  display.u8g2.setFont(u8g2_font_logisoso24_tr);
  display.u8g2.setDrawColor(1);

  display.drawCenteredText(24, "Updating");

  switch (state)
  {
  case UpdateProgressState::GETTING_READY: // Getting ready
    display.u8g2.setFont(u8g2_font_logisoso16_tr);
    display.drawCenteredText(64, "Getting ready...");
    break;

  case UpdateProgressState::UPDATING: // Updating
    display.u8g2.setFont(u8g2_font_profont12_tf);
    display.drawCenteredText(44, String(progress) + "%");

    display.u8g2.drawFrame(0, 48, 127, 16);
    display.u8g2.drawBox(2, 50, map(progress, 0, 100, 0, 123), 12);
    break;

  case UpdateProgressState::SHOWING_MESSAGE: // Showing message
    display.u8g2.setFont(u8g2_font_logisoso16_tr);
    display.drawCenteredText(50, msg);
    break;

  default:
    break;
  }
}

void UpdateProgressScreen::setProgress(uint8_t _progress)
{
  state = UpdateProgressState::UPDATING;
  progress = _progress;
}

void UpdateProgressScreen::setMessage(String _msg)
{
  state = UpdateProgressState::SHOWING_MESSAGE;
  msg = _msg;
}
