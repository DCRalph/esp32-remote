#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include "IO/Wireless.h"

class BoxThingEncoderScreen : public Screen
{
public:
  BoxThingEncoderScreen(String _name);

  uint64_t encCount = 0;
  int8_t clicks = 0;
  int8_t lastClicks = 0;

  void onRecv(fullPacket *fp);

  void draw() override;
  void update() override;
};

BoxThingEncoderScreen::BoxThingEncoderScreen(String _name) : Screen(_name)
{
}

void BoxThingEncoderScreen::onRecv(fullPacket *fp)
{
  memcpy(&encCount, fp->p.data, 8);
  memcpy(&clicks, fp->p.data + 8, 1);
  if (clicks != 0)
  {
    lastClicks = clicks;
  }
}

void BoxThingEncoderScreen::draw()
{
  display.noTopBar();
  display.setTextSize(4);
  display.setTextDatum(TC_DATUM);
  display.setTextColor(TFT_WHITE);
  display.drawString("Encoder", LCD_WIDTH / 2, 60);

  char buf[20];
  sprintf(buf, "%d", lastClicks);
  display.drawString(buf, LCD_WIDTH / 2, 100);

  sprintf(buf, "%d", encCount);
  display.drawString(buf, LCD_WIDTH / 2, 140);
}

void BoxThingEncoderScreen::update()
{
  if (ClickButtonUP.clicks != 0 || ClickButtonDOWN.clicks != 0)
  {
    screenManager.back();
  }
}