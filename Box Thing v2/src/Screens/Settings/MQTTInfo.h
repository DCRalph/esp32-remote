#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

class MQTTInfoScreen : public Screen
{
public:
  MQTTInfoScreen(String _name);

  void draw() override;
  void update() override;
};

MQTTInfoScreen::MQTTInfoScreen(String _name) : Screen(_name)
{
}

void MQTTInfoScreen::draw()
{
}

void MQTTInfoScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }
}