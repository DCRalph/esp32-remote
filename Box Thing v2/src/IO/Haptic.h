#pragma once

#include "config.h"
#include "Adafruit_DRV2605.h"

class Haptic
{
private:
  Adafruit_DRV2605 drv;

public:
  void init();

  void playEffect(uint8_t effect);

  void stopEffect();
};

extern Haptic haptic;