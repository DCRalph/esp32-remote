#include "IndicatorEffect.h"
#include <Arduino.h>
#include <math.h>

IndicatorEffect::IndicatorEffect(LEDManager *_ledManager, Side side, uint8_t priority,
                                 bool transparent)
    : LEDEffect(_ledManager, priority, transparent),
      side(side),
      indicatorActive(false),
      fadeFactor(0.0f),
      // Default parameters – feel free to adjust.
      blinkCycle(1200),
      fadeInTime(250)
{
  activatedTime = 0;
  otherIndicator = nullptr;
  synced = false;

  // Default color is amber/yellow.
  baseR = 255;
  baseG = 120;
  baseB = 0;
}

void IndicatorEffect::setOtherIndicator(IndicatorEffect *otherIndicator)
{
  this->otherIndicator = otherIndicator;
}

IndicatorEffect *IndicatorEffect::getOtherIndicator() const
{
  return otherIndicator;
}

void IndicatorEffect::setIndicatorActive(bool active)
{
  // If state is unchanged, nothing to do.
  if (indicatorActive == active)
  {
    return;
  }

  indicatorActive = active;

  if (active)
  {
    // If another indicator exists and is active, sync start times.
    if (otherIndicator != nullptr && otherIndicator->getIndicatorActive())
    {
      syncWithOtherIndicator();
    }
    else
    {
      // Otherwise, simply set this activatedTime.
      activatedTime = millis();
    }
  }
  else
  {
    // Clear fade factor if the indicator is turned off.
    fadeFactor = 0.0f;
    activatedTime = 0;

    if (synced == true)
    {
      synced = false;
      if (otherIndicator != nullptr)
        otherIndicator->synced = false;
    }
  }
}

bool IndicatorEffect::getIndicatorActive() const
{
  return indicatorActive;
}

void IndicatorEffect::syncWithOtherIndicator()
{
  if (otherIndicator == nullptr)
  {
    return;
  }

  // Both indicators are active. We'll sync by choosing the earlier
  // activation time between the two.
  unsigned long commonTime = (activatedTime < otherIndicator->activatedTime)
                                 ? activatedTime
                                 : otherIndicator->activatedTime;

  activatedTime = commonTime;
  otherIndicator->activatedTime = commonTime;
  synced = true;
  otherIndicator->synced = true;
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
  uint64_t currentTime = millis();
  currentTime -= activatedTime;
  uint64_t timeInCycle = currentTime % blinkCycle;

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
  if (!indicatorActive)
    return;

  uint16_t regionLength = ledManager->getNumLEDs() / 5;
  // regionLength += 1;

  if (regionLength <= 1)
  {
    // Handle error or simply return
    return;
  }

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
      if (fadeFactor >= d)
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
      buffer[i].b = static_cast<uint8_t>(baseB * finalFactor);
    }
  }
  else
  {
    // For the right indicator, the region covers indices numLEDs - regionLength to numLEDs-1.
    // The inner edge is at index numLEDs - regionLength.
    uint16_t start = ledManager->getNumLEDs() - regionLength;
    for (uint16_t i = start; i < ledManager->getNumLEDs(); i++)
    {
      // Compute normalized distance from the inner edge.
      // For the right indicator, define:
      //    d = (i - start) / (regionLength - 1)
      // So d = 0 at the inner edge (i = start) and 1 at the outer edge (i = numLEDs-1).
      float d = (float)(i - start) / (float)(regionLength - 1);

      float finalFactor = 0.0f;
      if (fadeFactor >= d)
      {
        finalFactor = (fadeFactor - d) / (1.0f - d);
        if (finalFactor > 1.0f)
        {
          finalFactor = 1.0f;
        }
      }

      buffer[i].r = static_cast<uint8_t>(baseR * finalFactor);
      buffer[i].g = static_cast<uint8_t>(baseG * finalFactor);
      buffer[i].b = static_cast<uint8_t>(baseB * finalFactor);
    }
  }
}
