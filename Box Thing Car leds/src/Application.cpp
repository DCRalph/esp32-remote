// application.cpp

#include "Application.h"
#include "config.h"      // Contains NUM_LEDS, LEDS_PIN, etc.
#include "IO/Wireless.h" // Your wireless library
#include "FastLED.h"

//----------------------------------------------------------------------------

Application *Application::getInstance()
{
  static Application instance;
  return &instance;
}

//----------------------------------------------------------------------------
/*
 * Application constructor. Initialize pointers and set defaults.
 */
Application::Application()
{
  // Initialize input pointers.
  accOn = &input1;
  brake = &input2;
  leftIndicator = &input3;
  rightIndicator = &input4;
  reverse = &input5;

  // Set default states.
  accOnState = false;
  brakeState = false;
  leftIndicatorState = false;
  rightIndicatorState = false;
  reverseState = false;

  lastAccOn = 0;
}

/*
 * Application destructor. Clean up dynamic allocations.
 */
Application::~Application()
{
  delete brakeEffect;
  delete leftIndicatorEffect;
  delete rightIndicatorEffect;
  delete reverseLightEffect;
  delete rgbEffect;
  delete nightriderEffect;
  delete startupEffect;

  delete ledManager;
  ledManager = nullptr;
}

/*
 * Begin: Initializes the LED strip, effects, wireless, etc.
 */
void Application::begin()
{
  FastLED.addLeds<WS2812, LEDS_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);

  // Flash a test LED to indicate startup.
  leds[0] = CRGB(255, 0, 0);
  FastLED.show();
  delay(500);
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();

  // Create and assign the custom LED manager.
  ledManager = new LEDManager(NUM_LEDS);
  ledManager->setFPS(100);
  ledManager->setDrawFunction(drawLEDs);

  // Set each effect's LED manager pointer.
  brakeEffect = new BrakeLightEffect(ledManager, 8, false);
  leftIndicatorEffect = new IndicatorEffect(ledManager, IndicatorEffect::LEFT,
                                            10, true);
  rightIndicatorEffect = new IndicatorEffect(ledManager, IndicatorEffect::RIGHT,
                                             10, true);
  reverseLightEffect = new ReverseLightEffect(ledManager, 6, false);
  rgbEffect = new RGBEffect(ledManager, 2, false);
  nightriderEffect = new NightRiderEffect(ledManager, 2, false);
  startupEffect = new StartupEffect(ledManager, 4, false);

  leftIndicatorEffect->setOtherIndicator(rightIndicatorEffect);
  rightIndicatorEffect->setOtherIndicator(leftIndicatorEffect);

  // Add effects to the LED manager.
  ledManager->addEffect(brakeEffect);
  ledManager->addEffect(leftIndicatorEffect);
  ledManager->addEffect(rightIndicatorEffect);
  ledManager->addEffect(reverseLightEffect);
  ledManager->addEffect(rgbEffect);
  ledManager->addEffect(nightriderEffect);
  ledManager->addEffect(startupEffect);

  // Sequences
  unlockSequence = new BothIndicatorsSequence(1);
  lockSequence = new BothIndicatorsSequence(3);
  RGBFlickSequence = new IndicatorFlickSequence(IndicatorSide::LEFT_SIDE);
  nightRiderFlickSequence = new IndicatorFlickSequence(IndicatorSide::RIGHT_SIDE);
  brakeTapSequence3 = new BrakeTapSequence(3);

  unlockSequence->setActive(true);

  unlockSequence->setCallback([this]()
                              {
                                startupEffect->setActive(true);
                                unlockSequence->setActive(false);
                                //
                              });

  lockSequence->setActive(true);

  lockSequence->setCallback([this]()
                            {
                              startupEffect->setActive(false);
                              unlockSequence->setActive(true);
                              //
                            });

  RGBFlickSequence->setActive(true);
  RGBFlickSequence->setCallback([this]()
                                {
                                  rgbEffect->setActive(!rgbEffect->isActive());
                                  nightRiderFlickSequence->reset();

                                  nightriderEffect->setActive(false);
                                  //
                                });

  nightRiderFlickSequence->setActive(true);
  nightRiderFlickSequence->setCallback([this]()
                                       {
                                         nightriderEffect->setActive(!nightriderEffect->isActive());
                                         RGBFlickSequence->reset();

                                         rgbEffect->setActive(false);
                                         //
                                       });

  brakeTapSequence3->setActive(true);
  brakeTapSequence3->setCallback([this]()
                                 {
                                   // diable all special effects
                                   rgbEffect->setActive(false);
                                   nightriderEffect->setActive(false);

                                   enableNormalMode();
                                   //
                                 });
}

static float accVolt;
static float brakeVolt;
static float leftVolt;
static float rightVolt;
static float reverseVolt;

void Application::updateInputs()
{
  // ADC conversion constants and threshold for digital "on".
  const float ADC_MAX = 8192;
  const float ADC_REF_VOLTAGE = 3.3;
  const float DIVIDER_FACTOR = 10.0;
  const float VOLTAGE_THRESHOLD = 3;

  float smoothFactor = 0.5f;

  // Update filtered voltages using a simple low-pass filter.
  accVolt = (accVolt * smoothFactor) +
            (((float)accOn->analogRead() / ADC_MAX *
              ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
             (1 - smoothFactor));
  brakeVolt = (brakeVolt * smoothFactor) +
              (((float)brake->analogRead() / ADC_MAX *
                ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
               (1 - smoothFactor));
  leftVolt = (leftVolt * smoothFactor) +
             (((float)leftIndicator->analogRead() / ADC_MAX *
               ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
              (1 - smoothFactor));
  rightVolt = (rightVolt * smoothFactor) +
              (((float)rightIndicator->analogRead() / ADC_MAX *
                ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
               (1 - smoothFactor));
  reverseVolt = (reverseVolt * smoothFactor) +
                (((float)reverse->analogRead() / ADC_MAX *
                  ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
                 (1 - smoothFactor));

  // Determine digital state for each input.
  accOnState = accVolt > VOLTAGE_THRESHOLD;
  brakeState = brakeVolt > VOLTAGE_THRESHOLD;
  leftIndicatorState = leftVolt > VOLTAGE_THRESHOLD;
  rightIndicatorState = rightVolt > VOLTAGE_THRESHOLD;
  reverseState = reverseVolt > VOLTAGE_THRESHOLD;
}

/*
 * update():
 * Main loop update.
 */
void Application::update()
{

  // Update input states.
  updateInputs();

  brakeTapSequence3->setInput(brakeState);
  brakeTapSequence3->loop();

  switch (mode)
  {
  case ApplicationMode::NORMAL:
    handleNormalEffects();
    break;

  case ApplicationMode::TEST:
  {
    // reverseLightEffect->setActive(true);
    // brakeEffect->setIsReversing(true);

    // brakeEffect->setActive(io0.read());
    // leftIndicatorEffect->setActive(io0.read());
    // rightIndicatorEffect->setActive(io0.read());
    // reverseLightEffect->setActive(io0.read());
    // rgbEffect->setActive(io0.read());
    startupEffect->setActive(io0.read());
  }
  break;

  case ApplicationMode::REMOTE:
    handleRemoteEffects();
    break;

  case ApplicationMode::OFF:
  {
    // turn off all effects
    brakeEffect->setActive(false);
    leftIndicatorEffect->setActive(false);
    rightIndicatorEffect->setActive(false);
    reverseLightEffect->setActive(false);
    rgbEffect->setActive(false);
    startupEffect->setActive(false);
  }
  break;
  }

  // Update and draw LED effects.
  ledManager->updateEffects();
  ledManager->draw();
}

/*
 * setTestMode():
 * Allows external code to enable or disable test mode.
 */
void Application::enableNormalMode()
{
  mode = ApplicationMode::NORMAL;
}

void Application::enableTestMode()
{
  mode = ApplicationMode::TEST;
}

void Application::enableRemoteMode()
{
  mode = ApplicationMode::REMOTE;
}

void Application::enableOffMode()
{
  mode = ApplicationMode::OFF;
}

/*
 * handleNormalEffects():
 * Reads the current inputs and sets effects accordingly.
 *
 * Features:
 * - If ACC is off and both indicator inputs are active, enable the
 *   startup effect until ACC is turned on.
 * - If ACC is on:
 *     * Brake pressed => Brake effect on.
 *     * Left indicator on => Left indicator effect on.
 *     * Right indicator on => Right indicator effect on.
 *     * Reverse on => Reverse light effect on.
 * - Other effects (e.g., RGB effect) can be handled via wireless commands.
 */

void Application::handleNormalEffects()
{

  unsigned long currentTime = millis();

  unlockSequence->setInputs(accOnState, leftIndicatorState, rightIndicatorState);
  lockSequence->setInputs(accOnState, leftIndicatorState, rightIndicatorState);
  RGBFlickSequence->setInputs(accOnState, leftIndicatorState, rightIndicatorState);
  nightRiderFlickSequence->setInputs(accOnState, leftIndicatorState, rightIndicatorState);

  unlockSequence->loop();
  lockSequence->loop();
  RGBFlickSequence->loop();
  nightRiderFlickSequence->loop();

  if (lastAccOn != 0 && currentTime - lastAccOn > 1 * 60 * 1000)
  {
    lastAccOn = 0;
    unlockSequence->setActive(true);

    // turn off all effects
    brakeEffect->setActive(false);
    leftIndicatorEffect->setActive(false);
    rightIndicatorEffect->setActive(false);
    reverseLightEffect->setActive(false);
    rgbEffect->setActive(false);
    startupEffect->setActive(false);
  }

  if (lastAccOnState != accOnState)
  {
    lastAccOnState = accOnState;

    if (accOnState == false)
    {
      startupEffect->setActive(true);
    }
  }

  if (!accOnState)
  {
    // Since ACC is off, disable the other effects.
    brakeEffect->setActive(false);
    leftIndicatorEffect->setActive(false);
    rightIndicatorEffect->setActive(false);
    reverseLightEffect->setActive(false);
  }
  else
  {
    // When ACC is on, ensure the startup effect is turned off and reset flash tracking.
    startupEffect->setActive(false);
    // flashCounter = 0;
    // firstFlashTime = 0;
    // lastCombinedIndicatorState = false;

    // And process the other effects normally.
    brakeEffect->setActive(brakeState);
    brakeEffect->setIsReversing(reverseState || reverseLightEffect->isAnimating());

    leftIndicatorEffect->setActive(leftIndicatorState);
    rightIndicatorEffect->setActive(rightIndicatorState);
    reverseLightEffect->setActive(reverseState);
  }
}

void Application::handleRemoteEffects()
{
}

void Application::drawLEDs()
{
  auto buf = Application::getInstance()->ledManager->getBuffer();
  uint16_t numLEDs = Application::getInstance()->ledManager->getNumLEDs();
  for (int i = 0; i < numLEDs; i++)
  {
    Application::getInstance()->leds[i] =
        CRGB(buf[i].r, buf[i].g, buf[i].b);
  }
  FastLED.show();
}
