#include "IndicatorEffect.h"
#include <Arduino.h>
#include <math.h>

IndicatorEffect::IndicatorEffect(uint16_t numLEDs, Side side, uint8_t priority,
                                 bool transparent)
    : LEDEffect(priority, transparent),
      numLEDs(numLEDs),
      side(side),
      indicatorActive(false),
      fadeFactor(0.0f),
      // Default parameters – feel free to adjust.
      blinkCycle(1200), 
      fadeInTime(300),
      baseR(255),
      baseG(120),
      baseB(0)
{
}

void IndicatorEffect::setIndicatorActive(bool active)
{
  indicatorActive = active;
  if (!active)
  {
    // When deactivated, reset fade factor.
    fadeFactor = 0.0f;
  }
}

void IndicatorEffect::update()
{
  if (!indicatorActive)
  {
    // Ensure fade factor remains 0 if not active.
    fadeFactor = 0.0f;
    return;
  }
  // Compute where we are within the blink cycle.
  unsigned long currentTime = millis();
  unsigned long timeInCycle = currentTime % blinkCycle;

  // For this design, the indicator is "on" only during the fade-in period.
  if (timeInCycle < (blinkCycle / 2))
  {
    // Fade factor increases linearly from 0 to 1 over fadeInTime.
    fadeFactor = (float)timeInCycle / fadeInTime;

    if (fadeFactor > 1.0f)
    {
      // Ensure fade factor is exactly 1 at the end of the fade-in period.
      fadeFactor = 1.0f;
    }
  }
  else
  {
    // Turn indicator off for the remainder of the blink cycle.
    fadeFactor = 0.0f;
  }
}

void IndicatorEffect::render(std::vector<Color> &buffer)
{
  // Do nothing if the indicator is inactive or the fade factor is 0.
  if (!indicatorActive || fadeFactor <= 0.0f)
    return;

  // Define the indicator region length as one quarter of the LED strip.
  uint16_t regionLength = numLEDs / 5;

  // For the left indicator, the region covers indices 0 to regionLength-1.
  // We want the "inner" edge (the side facing the vehicle) to light first.
  // For the left indicator, that is index (regionLength-1).
  if (side == LEFT)
  {
    for (uint16_t i = 0; i < regionLength; i++)
    {
      // Compute normalized distance from the inner edge.
      // For left indicator, define:
      //    d = (regionLength-1 - i) / (regionLength-1)
      // So d = 0 at the inner edge (i = regionLength-1) and 1 at the outer edge (i = 0).
      float d = (float)(regionLength - 1 - i) / (float)(regionLength - 1);

      // Determine when LED i should begin lighting.
      // Here we use d directly as a threshold: the inner LED (d=0) lights up immediately,
      // while an outer LED (d close to 1) lights only when fadeFactor is nearly 1.
      float finalFactor = 0.0f;
      if (fadeFactor > d)
      {
        // Map fadeFactor onto [0,1] for this LED.
        finalFactor = (fadeFactor - d) / (1.0f - d);
        if (finalFactor > 1.0f)
        {
          finalFactor = 1.0f;
        }
      }

      // Apply the final factor to the desired base color.
      buffer[i].r = static_cast<uint8_t>(baseR * finalFactor);
      buffer[i].g = static_cast<uint8_t>(baseG * finalFactor);
      buffer[i].b = baseB;
    }
  }
  else
  {
    // For the right indicator, the region covers indices numLEDs - regionLength to numLEDs-1.
    // The inner edge is at index numLEDs - regionLength.
    uint16_t start = numLEDs - regionLength;
    for (uint16_t i = start; i < numLEDs; i++)
    {
      // Compute normalized distance from the inner edge.
      // For the right indicator, define:
      //    d = (i - start) / (regionLength - 1)
      // So d = 0 at the inner edge (i = start) and 1 at the outer edge (i = numLEDs-1).
      float d = (float)(i - start) / (float)(regionLength - 1);

      float finalFactor = 0.0f;
      if (fadeFactor > d)
      {
        finalFactor = (fadeFactor - d) / (1.0f - d);
        if (finalFactor > 1.0f)
        {
          finalFactor = 1.0f;
        }
      }

      buffer[i].r = static_cast<uint8_t>(baseR * finalFactor);
      buffer[i].g = static_cast<uint8_t>(baseG * finalFactor);
      buffer[i].b = baseB;
    }
  }
}
