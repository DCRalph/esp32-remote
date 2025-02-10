#pragma once

#include "../Effects.h"
#include <stdint.h>

class BrakeLightEffect : public LEDEffect
{
public:
  // Constructs a brake light effect for a given number of LEDs.
  BrakeLightEffect(uint16_t numLEDs, uint8_t priority = 0,
                   bool transparent = false);
  virtual void update() override;
  virtual void render(std::vector<Color> &buffer) override;

  // Set whether the brakes are active.
  void setBrakeActive(bool active);

private:
  uint16_t numLEDs;
  unsigned long lastUpdate;

  bool brakeActive;
  float brightness; // Overall brightness (1.0 = full red)
  float size;       // Size of the brake light effect.
};
