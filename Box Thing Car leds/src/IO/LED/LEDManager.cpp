#include "LEDManager.h"
#include <algorithm>

// Implementation of the Color structure.
// range of h: [0, 360), s: [0, 1], v: [0, 1]
// returns: RGB color with components in the range [0, 255].
Color Color::hsv2rgb(float h, float s, float v)
{
  // Convert HSV to RGB color space.
  float r, g, b;
  int i;
  float f, p, q, t;

  if (s == 0)
  {
    // Achromatic (grey)
    r = g = b = v;
    return Color(r * 255, g * 255, b * 255);
  }

  h /= 60; // sector 0 to 5
  i = floor(h);
  f = h - i; // factorial part of h
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));

  switch (i)
  {
  case 0:
    r = v;
    g = t;
    b = p;
    break;
  case 1:
    r = q;
    g = v;
    b = p;
    break;
  case 2:
    r = p;
    g = v;
    b = t;
    break;
  case 3:
    r = p;
    g = q;
    b = v;
    break;
  case 4:
    r = t;
    g = p;
    b = v;
    break;
  default: // case 5:
    r = v;
    g = p;
    b = q;
    break;
  }

  return Color(r * 255, g * 255, b * 255);
}

LEDManager::LEDManager(uint16_t numLEDs)
    : numLEDs(numLEDs), ledBuffer(numLEDs)
{
  fps = 100;
}

LEDManager::~LEDManager()
{
  // Clean up all effects – if dynamically allocated, delete them.
  for (auto effect : effects)
  {
    delete effect;
  }
  effects.clear();
}

void LEDManager::addEffect(LEDEffect *effect)
{
  effects.push_back(effect);
  // Sort effects by priority so that lower priority effects are rendered first.
  std::sort(effects.begin(), effects.end(),
            [](const LEDEffect *a, const LEDEffect *b)
            {
              return a->getPriority() < b->getPriority();
            });
}

void LEDManager::removeEffect(LEDEffect *effect)
{
  effects.erase(std::remove(effects.begin(), effects.end(), effect),
                effects.end());
}

void LEDManager::updateEffects()
{
  if (lastUpdateTime == 0)
  {
    lastUpdateTime = millis();
  }

  // Calculate the time since the last update.
  uint64_t currentTime = millis();
  uint64_t deltaTime = currentTime - lastUpdateTime;

  // Calculate the time per frame in milliseconds.
  uint64_t frameTime = 1000 / fps;

  // If the time since the last update is greater than the frame time,
  // update the effects and reset the last update time.
  if (deltaTime >= frameTime)
  {
    lastUpdateTime = currentTime;
  }
  else
  {
    return;
  }

  // Clear the buffer to black before applying effects.
  clearBuffer();

  // Update and then render each effect. Effects are rendered sequentially;
  // higher-priority effects can override the LED values.
  for (auto effect : effects)
  {
    effect->update();
    effect->render(ledBuffer);
  }
}

void LEDManager::draw()
{
  // Placeholder – integration with the actual LED library goes here.
  // E.g., you could update a FastLED or Adafruit NeoPixel strip.
}

std::vector<Color> &LEDManager::getBuffer() { return ledBuffer; }

void LEDManager::clearBuffer()
{
  for (auto &led : ledBuffer)
  {
    led.r = 0;
    led.g = 0;
    led.b = 0;
  }
}

uint16_t LEDManager::getNumLEDs() const { return numLEDs; }

void LEDManager::setFPS(uint16_t fps) { this->fps = fps; }

uint16_t LEDManager::getFPS() const { return fps; }