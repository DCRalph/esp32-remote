#include "BrakeLightEffect.h"
#include <cmath>

BrakeLightEffect::BrakeLightEffect(LEDManager *_ledManager, uint8_t priority,
                                   bool transparent)
    : LEDEffect(_ledManager, priority, transparent),
      lastUpdate(0),
      brakeActive(false),
      brightness(0.0f),
      size(0.0f) {}

void BrakeLightEffect::setBrakeActive(bool active)
{
  if (active)
  {
    // Immediately set full brightness when brakes are applied.
    brakeActive = true;
    brightness = 1.0f;
    size = 1.0f;
  }
  else
  {
    brakeActive = false;
    brightness = 0.3f;
  }
}

void BrakeLightEffect::update()
{
  // Get the current time in milliseconds.
  unsigned long currentTime = millis();

  // Initialize lastUpdate on the very first call.
  if (lastUpdate == 0)
  {
    lastUpdate = currentTime;
    return;
  }

  // Compute the elapsed time (dt) in seconds.
  unsigned long dtMillis = currentTime - lastUpdate;
  float dtSeconds = dtMillis / 1000.0f;
  lastUpdate = currentTime;

  // Define the dimming rates.
  const float BRIGHTNESS_DIM_RATE = 1.0f; // brightness reduction per second
  const float SIZE_DIM_RATE = 0.9f;       // reduction in lit area (from edges)

  if (!brakeActive)
  {
    // When brakes are released, reduce brightness in a nonlinear fashion.
    // Multiplying by brightness causes the drop to slow as brightness nears 0.
    brightness -= BRIGHTNESS_DIM_RATE * pow(brightness, 0.5f) * dtSeconds;
    if (brightness < 0.0f)
      brightness = 0.0f;

    // Reduce the size of the fully lit central region linearly.
    size -= SIZE_DIM_RATE * dtSeconds;
    if (size < 0.0f)
      size = 0.0f;
  }
  else
  {
    // When brakes are pressed, restore full brightness and size.
    brightness = 1.0f;
    size = 1.0f;
  }
}

void BrakeLightEffect::render(std::vector<Color> &buffer)
{

  if (brakeActive)
  {
    // With brakes active, show a solid red (full brightness) across all LEDs.
    for (uint16_t i = 0; i < ledManager->getNumLEDs(); i++)
    {
      buffer[i].r = 255;
      buffer[i].g = 0;
      buffer[i].b = 0;
    }
  }
  else if (brightness > 0.0f && size > 0.0f)
  {
    // When brakes are released, show a gradient that fades out from the edges
    // toward the center as the 'size' value decreases.
    // The LEDs inside the cutoff (defined by 'size') remain at full brightness
    // (scaled by the overall 'brightness'), while those outside fade aggressively.
    uint16_t mid = ledManager->getNumLEDs() / 2;
    const float rollOffExponent = 3.0f; // Higher values yield a steeper fade

    for (uint16_t i = 0; i < ledManager->getNumLEDs(); i++)
    {
      // Calculate a normalized distance from the center (0 at center, 1 at edges).
      float normDist = (mid > 0)
                           ? fabs((int)i - (int)mid) / static_cast<float>(mid)
                           : 0.0f;

      float localBrightness = brightness; // Base brightness for this LED.

      // if the led is bigger than the size, reduce the brightness
      if (normDist > size)
      {
        // Calculate the normalized distance beyond the cutoff.
        float distBeyond = (normDist - size) / (1.0f - size);
        // Apply a nonlinear roll-off to the brightness.
        localBrightness *= pow(1.0f - distBeyond, rollOffExponent);
      }

      // Calculate the red channel intensity.
      uint8_t redVal = static_cast<uint8_t>(255 * localBrightness);
      buffer[i].r = redVal;
      buffer[i].g = 0;
      buffer[i].b = 0;
    }
  }
}