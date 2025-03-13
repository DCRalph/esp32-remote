

#include "Haptic.h"

void Haptic::init()
{
  if (!drv.begin())
  {
    Serial.println("Could not find DRV2605");
    while (1)
      delay(10);
  }

  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG);
}

void Haptic::playEffect(uint8_t effect)
{
  drv.setWaveform(0, effect);
  drv.setWaveform(1, 0);

  drv.go();
}

void Haptic::stopEffect()
{
  drv.stop();
}

Haptic haptic;