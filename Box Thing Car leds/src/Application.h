// application.h

#pragma once

#include "config.h"
#include "FastLED.h"
#include "IO/LED/LEDManager.h"
#include "IO/LED/Effects/BrakeLightEffect.h"
#include "IO/LED/Effects/IndicatorEffect.h"
#include "IO/LED/Effects/ReverseLightEffect.h"
#include "IO/LED/Effects/RGBEffect.h"
#include "IO/LED/Effects/StartupEffect.h"

// Custom LED manager that abstracts LEDManager::draw()
class CustomLEDManager : public LEDManager
{
public:
  CustomLEDManager(uint16_t numLEDs) : LEDManager(numLEDs) {}

  virtual void draw() override;
};

class Application
{
public:
  static Application *getInstance();

  Application();
  ~Application();

  // Call once to initialize the system.
  void begin();

  // Main update function to be called from loop()
  void update();

  // Set testMode externally.
  void setTestMode(bool mode);

  // Global pointer to the custom LED manager.
  CRGB leds[NUM_LEDS];

  CustomLEDManager *ledManager;

private:
  int analogThreshold = 100;

  // Input pointers.
  GpIO *accOn;          // 12v ACC
  GpIO *brake;          // Brake
  GpIO *leftIndicator;  // Left indicator
  GpIO *rightIndicator; // Right indicator
  GpIO *reverse;        // Reverse

  // Effect instances.
  BrakeLightEffect *brakeEffect;
  IndicatorEffect *leftIndicatorEffect;
  IndicatorEffect *rightIndicatorEffect;
  ReverseLightEffect *reverseLightEffect;
  RGBEffect *rgbEffect;
  StartupEffect *startupEffect;

  // Test mode flag. When true, the code ignores physical inputs.
  bool testMode;


  // Internal method to handle effect selection based on inputs.
  void handleEffects();
};
