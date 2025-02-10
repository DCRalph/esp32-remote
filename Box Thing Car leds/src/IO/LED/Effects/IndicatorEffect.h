#pragma once

#include "../Effects.h"
#include <stdint.h>

// Fancy, blink-based indicator effect that fades in from the center edge.
// The indicator region is defined as a quarter of the LED strip.
// The left indicator’s inner edge is at index (regionLength-1) and the right indicator’s
// inner edge is at index (numLEDs - regionLength).
class IndicatorEffect : public LEDEffect {
public:
  enum Side { LEFT = 0, RIGHT = 1 };

  // Constructs an indicator effect for a given LED strip.
  // All timing and color parameters are customizable.
  IndicatorEffect(uint16_t numLEDs, Side side, uint8_t priority = 1,
                  bool transparent = false);

  virtual void update() override;
  virtual void render(std::vector<Color>& buffer) override;

  // Activate or deactivate the indicator.
  void setIndicatorActive(bool active);

  // Customizable parameters:
  unsigned long blinkCycle;    // Total blink period in ms (default: 1000)
  unsigned long fadeInTime;      // Duration of fade in (ms) during the on phase (default: 300)
  // Base color for the indicator (default: amber/yellow).
  uint8_t baseR;
  uint8_t baseG;
  uint8_t baseB;

private:
  uint16_t numLEDs;
  Side side;
  bool indicatorActive;

  // Computed fade factor (0.0 to 1.0) for the current blink cycle.
  // It increases linearly during the fade in period and then resets.
  float fadeFactor;
};
