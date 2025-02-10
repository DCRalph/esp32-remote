#pragma once

#include <stdint.h>
#include <vector>
#include "Effects.h"
#include <Arduino.h>

// A simple structure representing an RGB color.
struct Color
{
  uint8_t r;
  uint8_t g;
  uint8_t b;

  Color() : r(0), g(0), b(0) {}
  Color(uint8_t red, uint8_t green, uint8_t blue)
      : r(red), g(green), b(blue) {}
};

class LEDEffect; // Forward declaration of effect class

class LEDManager
{
public:
  // Constructor that allocates an LED buffer based on the number of LEDs.
  LEDManager(uint16_t numLEDs);
  virtual ~LEDManager();

  // Add an effect to the manager.
  // Effects with higher priority override lower-priority ones.
  void addEffect(LEDEffect *effect);

  // Remove an effect
  void removeEffect(LEDEffect *effect);

  // Update all effects (call update() and then render() for each effect)
  void updateEffects();

  // Output the current LED buffer to the physical LED strip.
  // This function is a placeholder – you should override it with your LED library code.
  virtual void draw();

  // Access the internal LED buffer.
  std::vector<Color> &getBuffer();

  // Clear the LED buffer (set all LEDs to black/off)
  void clearBuffer();

  // Get the total number of LEDs.
  uint16_t getNumLEDs() const;

  void setFPS(uint16_t fps);

  uint16_t getFPS() const;

protected:
  uint16_t numLEDs;
  uint16_t fps;
  uint64_t lastUpdateTime;
  std::vector<Color> ledBuffer;
  std::vector<LEDEffect *> effects;
};
