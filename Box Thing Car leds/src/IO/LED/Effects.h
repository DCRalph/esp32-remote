#pragma once

#include "LEDManager.h"
#include <vector>
#include <stdint.h>

struct Color;

// Base class for LED effects.
class LEDEffect
{
public:
  // Each effect has a priority and may be "transparent" so that lower layers show
  LEDEffect(uint8_t priority = 0, bool transparent = false);
  virtual ~LEDEffect();

  // Called each update to change animation state.
  virtual void update() = 0;

  // Called to render the effect into the provided LED buffer.
  virtual void render(std::vector<Color> &buffer) = 0;

  uint8_t getPriority() const;
  bool isTransparent() const;
  void setPriority(uint8_t priority);
  void setTransparent(bool transparent);

protected:
  uint8_t priority;
  bool transparent;
};