#include "ReverseLightEffect.h"

ReverseLightEffect::ReverseLightEffect(LEDManager *_ledManager, uint8_t priority,
                                       bool transparent)
    : LEDEffect(_ledManager, priority, transparent), active(false) {}

void ReverseLightEffect::setActive(bool _active) {
  active = _active;
}

bool ReverseLightEffect::isActive() const {
  return active;
}

void ReverseLightEffect::update() {
  // Nothing to update if not active.
  if (!active)
    return;
  // No update necessary for a static solid-white effect.
}

void ReverseLightEffect::render(std::vector<Color> &buffer) {
  if (!active)
    return;
  // Set every LED to white.
  uint16_t num = ledManager->getNumLEDs();
  for (uint16_t i = 0; i < num; i++) {
    buffer[i].r = 255;
    buffer[i].g = 255;
    buffer[i].b = 255;
  }
}
