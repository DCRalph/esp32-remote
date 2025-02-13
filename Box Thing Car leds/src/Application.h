// application.h

#pragma once

#include "config.h"
#include "FastLED.h"
#include "IO/LED/LEDManager.h"
#include "IO/LED/Effects/BrakeLightEffect.h"
#include "IO/LED/Effects/IndicatorEffect.h"
#include "IO/LED/Effects/ReverseLightEffect.h"
#include "IO/LED/Effects/RGBEffect.h"
#include "IO/LED/Effects/NightRiderEffect.h"
#include "IO/LED/Effects/StartupEffect.h"

#include "Sequences/SequenceBase.h"
#include "Sequences/BothIndicatorsSequence.h"
#include "Sequences/IndicatorFlickSequence.h"
#include "Sequences/BrakeTapSequence.h"

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

  LEDManager *ledManager;

private:
  // Input pointers.
  GpIO *accOn;          // 12v ACC
  GpIO *brake;          // Brake
  GpIO *leftIndicator;  // Left indicator
  GpIO *rightIndicator; // Right indicator
  GpIO *reverse;        // Reverse

  uint64_t lastAccOn;

  // Effect instances.
  BrakeLightEffect *brakeEffect;
  IndicatorEffect *leftIndicatorEffect;
  IndicatorEffect *rightIndicatorEffect;
  ReverseLightEffect *reverseLightEffect;
  RGBEffect *rgbEffect;
  NightRiderEffect *nightriderEffect;
  StartupEffect *startupEffect;

  // Sequences
  BothIndicatorsSequence *unlockSequence;
  BothIndicatorsSequence *lockSequence;
  IndicatorFlickSequence *RGBFlickSequence;
  IndicatorFlickSequence *nightRiderFlickSequence;
  BrakeTapSequence *brakeTapSequence3;


  // Test mode flag. When true, the code ignores physical inputs.
  bool testMode;

  // Internal method to handle effect selection based on inputs.
  void handleEffects();

  static void drawLEDs();
};
