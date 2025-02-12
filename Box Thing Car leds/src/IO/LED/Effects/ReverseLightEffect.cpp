#include "ReverseLightEffect.h"
#include <Arduino.h>
#include <math.h>

// Ease in/out function using a cosine interpolation.
// t: input value between 0 and 1.
static float easeInOut(float t) {
  return 0.5f * (1.0f - cosf(t * PI));
}

ReverseLightEffect::ReverseLightEffect(LEDManager *_ledManager, uint8_t priority, bool transparent)
    : LEDEffect(_ledManager, priority, transparent),
      active(false),
      animationSpeed(2.0f),   // default 2 seconds for a full animation cycle
      started(false),
      progress(0.0f),
      startTime(0)
{
}

void ReverseLightEffect::setActive(bool _active) {
  if(active == _active)
    return;
  active = _active;
  if(active) {
    // Reset state to start the animation anew.
    started = false;
    progress = 0.0f;
  }
}

bool ReverseLightEffect::isActive() const {
  return active;
}


void ReverseLightEffect::update() {
  // Only update if active and animation is not complete.
  if (!active || progress >= 1.0f)
    return;

  if (!started) {
    startTime = millis();
    started = true;
  }

  // Calculate progress from 0 to 1 based on animationSpeed.
  uint64_t elapsed = millis() - startTime;
  progress = (float)elapsed / (animationSpeed * 1000.0f);
  if (progress > 1.0f)
    progress = 1.0f;
}

void ReverseLightEffect::render(std::vector<Color> &buffer) {
  // If not active or animation complete, do not modify the buffer.
  if (!active || progress >= 1.0f)
    return;

  uint16_t numLEDs = ledManager->getNumLEDs();
  // Compute the center index (for even number, approximate center).
  float center = (numLEDs - 1) / 2.0f;
  // Maximum normalized distance from center.
  float maxDist = center;

  // Compute a scaled factor based on progress.
  // For the first half of the animation, we animate out from the center,
  // and during the second half, we animate in from the edges.
  float curveFactor = 0.0f;
  if (progress < 0.5f) {
    // Scale progress to [0,1] for the "expanding" phase.
    float p = progress * 2.0f;
    curveFactor = easeInOut(p);
  }
  else {
    // Scale progress to [0,1], inverted, for the "contracting" phase.
    float p = (1.0f - progress) * 2.0f;
    curveFactor = easeInOut(p);
  }

  // For each LED, determine whether it is lit based on its distance from the center.
  for (uint16_t i = 0; i < numLEDs; i++) {
    // Normalize the distance from the center [0, 1]
    float distance = fabs(i - center) / maxDist;
    if (distance <= curveFactor) {
      // LED is within the region to be lit.
      // Here we set full white.  You could also scale brightness based on distance if desired.
      buffer[i].r = 255;
      buffer[i].g = 255;
      buffer[i].b = 255;
    } 
    else {
      // Do not affect LEDs outside the current animation region.
      // (Leave existing contents unchanged.)
    }
  }
}
