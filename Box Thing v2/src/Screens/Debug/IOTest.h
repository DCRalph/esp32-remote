#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

class IOTestScreen : public Screen
{
public:
  IOTestScreen(String _name);

  void draw() override;
  void update() override;

  int lastEncoderClicks;
};

IOTestScreen::IOTestScreen(String _name) : Screen(_name)
{
  lastEncoderClicks = 0;
}

void IOTestScreen::draw()
{
  if (ClickButtonEnc.clicks != 0)
  {
    lastEncoderClicks = ClickButtonEnc.clicks;
  }

  display.u8g2.setFont(u8g2_font_logisoso16_tf);
  display.u8g2.setDrawColor(1);

  Serial.println(encoder.getCount());
  Serial.println(encoderGetCount());

  int count1 = encoder.getCount();
  int count2 = encoderGetCount();

  char buffer[64];
  sprintf(buffer, "Enc: %i:%i", count1, count2);
  display.u8g2.drawStr(0, 28, buffer);

  sprintf(buffer, "Btn: %d : %d", ClickButtonEnc.depressed, lastEncoderClicks);
  display.u8g2.drawStr(0, 46, buffer);

  // sprintf(buffer, "Switch: %s", switchPin.read() ? "ON" : "OFF");
  // display.u8g2.drawStr(0, 64, buffer);
}

void IOTestScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }

  if (ClickButtonEnc.clicks == 3)
  {
    encoder.clearCount();
  }
}