#include "RGBEffect.h"
#include <cmath>

RGBEffect::RGBEffect(LEDManager *_ledManager, uint8_t priority, bool transparent)
    : LEDEffect(_ledManager, priority, transparent),
      active(false),
      baseHueCenter(0.0f),  // Default center hue is red.
      baseHueEdge(300.0f),  // Default edge hue is violet.
      speed(60.0f),         // Default speed: 60 degrees per second.
      hueOffset(0.0f),
      lastUpdateTime(0)
{
  // Initialize the animated hues to the base values.
  hueCenter = baseHueCenter;
  hueEdge = baseHueEdge;
}

void RGBEffect::setActive(bool _active) {
  active = _active;
  if (_active && lastUpdateTime == 0) {
    lastUpdateTime = millis();
  }
}

bool RGBEffect::isActive() const {
  return active;
}

void RGBEffect::update() {
  // Return if the effect is not active.
  if (!active)
    return;

  unsigned long currentTime = millis();
  // Initialize last update if needed.
  if (lastUpdateTime == 0) {
    lastUpdateTime = currentTime;
    return;
  }
  
  // Calculate elapsed time in seconds.
  unsigned long dtMillis = currentTime - lastUpdateTime;
  float dtSeconds = dtMillis / 1000.0f;
  lastUpdateTime = currentTime;
  
  // Update the cumulative hue offset.
  // speed is in degrees per second.
  hueOffset += speed * dtSeconds;
  // Wrap hueOffset to the range [0, 360).
  hueOffset = fmod(hueOffset, 360.0f);

  // Adjust the animated hues based on the base values and the offset.
  hueCenter = baseHueCenter + hueOffset;
  hueEdge = baseHueEdge + hueOffset;

  // Wrap animated hues within the [0,360) range.
  while (hueCenter < 0.0f)
    hueCenter += 360.0f;
  while (hueCenter >= 360.0f)
    hueCenter -= 360.0f;

  while (hueEdge < 0.0f)
    hueEdge += 360.0f;
  while (hueEdge >= 360.0f)
    hueEdge -= 360.0f;
}

void RGBEffect::render(std::vector<Color> &buffer) {
  if (!active)
    return;
    
  uint16_t num = ledManager->getNumLEDs();
  uint16_t mid = num / 2;

  // For every LED, compute the normalized distance from the center.
  // At the center (distance=0) use hueCenter; at the edges (distance=1) use hueEdge.
  for (uint16_t i = 0; i < num; i++) {
    float normDist = (mid > 0)
                         ? fabs((int)i - (int)mid) / static_cast<float>(mid)
                         : 0.0f;
    // Linearly interpolate the hue from hueCenter to hueEdge.
    float hue = hueCenter + (hueEdge - hueCenter) * normDist;
    // Wrap hue within [0,360).
    while (hue < 0.0f)
      hue += 360.0f;
    while (hue >= 360.0f)
      hue -= 360.0f;

    uint8_t r, g, b;
    HsvToRgb(hue, 1.0f, 1.0f, r, g, b);
    buffer[i].r = r;
    buffer[i].g = g;
    buffer[i].b = b;
  }
}

void RGBEffect::HsvToRgb(float h, float s, float v,
                         uint8_t &outR, uint8_t &outG, uint8_t &outB) {
  float c = v * s;
  float x = c * (1 - fabs(fmod(h / 60.0f, 2) - 1));
  float m = v - c;
  float r_p, g_p, b_p;

  if (h < 60) {
    r_p = c;
    g_p = x;
    b_p = 0;
  } else if (h < 120) {
    r_p = x;
    g_p = c;
    b_p = 0;
  } else if (h < 180) {
    r_p = 0;
    g_p = c;
    b_p = x;
  } else if (h < 240) {
    r_p = 0;
    g_p = x;
    b_p = c;
  } else if (h < 300) {
    r_p = x;
    g_p = 0;
    b_p = c;
  } else {
    r_p = c;
    g_p = 0;
    b_p = x;
  }

  outR = static_cast<uint8_t>((r_p + m) * 255);
  outG = static_cast<uint8_t>((g_p + m) * 255);
  outB = static_cast<uint8_t>((b_p + m) * 255);
}
