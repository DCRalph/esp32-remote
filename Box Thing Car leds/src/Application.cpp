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
    : testMode(false)
{
  // Initialize input pointers.
  accOn = &input1;
  brake = &input2;
  leftIndicator = &input3;
  rightIndicator = &input4;
  reverse = &input5;
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
  rgbEffect = new RGBEffect(ledManager, 0, false);
  startupEffect = new StartupEffect(ledManager, 4, false);

  leftIndicatorEffect->setOtherIndicator(rightIndicatorEffect);
  rightIndicatorEffect->setOtherIndicator(leftIndicatorEffect);

  // Add effects to the LED manager.
  ledManager->addEffect(brakeEffect);
  ledManager->addEffect(leftIndicatorEffect);
  ledManager->addEffect(rightIndicatorEffect);
  ledManager->addEffect(reverseLightEffect);
  ledManager->addEffect(rgbEffect);
  ledManager->addEffect(startupEffect);

  // Sequences
  unlockSequence = new BothIndicatorsSequence(1);
  lockSequence = new BothIndicatorsSequence(3);
  flickSequence = new IndicatorFlickSequence();

  unlockSequence->setActive(true);

  unlockSequence->setCallback([this]()
                              {
                                startupEffect->setActive(true);
                                unlockSequence->setActive(false);
                                lockSequence->setActive(true);
                                //
                              });

  lockSequence->setCallback([this]()
                            {
                              startupEffect->setActive(false);
                              lockSequence->setActive(false);
                              unlockSequence->setActive(true);
                              //
                            });

  flickSequence->setActive(true);
  flickSequence->setCallback([this]()
                             {
                               rgbEffect->setActive(!rgbEffect->isActive());
                               //
                             });
}

/*
 * update():
 * Main loop update.
 */
void Application::update()
{
  if (testMode)
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
  else
  {
    // Read inputs and decide which effects should be active.
    handleEffects();
  }

  // Update and draw LED effects.
  ledManager->updateEffects();
  ledManager->draw();
}

/*
 * setTestMode():
 * Allows external code to enable or disable test mode.
 */
void Application::setTestMode(bool mode)
{
  testMode = mode;
}

/*
 * handleEffects():
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

static float accVolt;
static float brakeVolt;
static float leftVolt;
static float rightVolt;
static float reverseVolt;

void Application::handleEffects()
{
  // ADC conversion constants and threshold for digital "on".
  const float ADC_MAX = 8192;
  const float ADC_REF_VOLTAGE = 3.3;
  const float DIVIDER_FACTOR = 10.0;
  const float VOLTAGE_THRESHOLD = 3;

  // Timing constants.
  // const unsigned long DEBOUNCE_MS = 50;         // debounce interval
  // const unsigned long FLASH_RESET_MS = 10000UL; // reset counter if 10 seconds pass
  // const int FLASHES_TO_ACTIVATE_STARTUP = 1;
  // const int FLASHES_TO_DEACTIVATE_STARTUP = 3; // 3 additional flashes to cancel startup

  // Static variables to persist between calls.
  // static bool lastCombinedIndicatorState = false; // previous combined indicator state
  // static unsigned long lastDebounceTime = 0;      // last debounce timestamp

  // To track flash events.
  // "firstFlashTime" marks the timestamp of the first flash event.
  // "flashCounter" counts the flashes, including the first one.
  // static unsigned long firstFlashTime = 0;
  // static int flashCounter = 0;

  // Update filtered voltages using a simple low-pass filter.
  accVolt = (accVolt * 0.9f) +
            (((float)accOn->analogRead() / ADC_MAX *
              ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
             0.1f);
  brakeVolt = (brakeVolt * 0.9f) +
              (((float)brake->analogRead() / ADC_MAX *
                ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
               0.1f);
  leftVolt = (leftVolt * 0.9f) +
             (((float)leftIndicator->analogRead() / ADC_MAX *
               ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
              0.1f);
  rightVolt = (rightVolt * 0.9f) +
              (((float)rightIndicator->analogRead() / ADC_MAX *
                ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
               0.1f);
  reverseVolt = (reverseVolt * 0.9f) +
                (((float)reverse->analogRead() / ADC_MAX *
                  ADC_REF_VOLTAGE * DIVIDER_FACTOR) *
                 0.1f);

  // Determine digital state for each input.
  bool accOnState = accVolt > VOLTAGE_THRESHOLD;
  bool brakeState = brakeVolt > VOLTAGE_THRESHOLD;
  bool leftIndicatorState = leftVolt > VOLTAGE_THRESHOLD;
  bool rightIndicatorState = rightVolt > VOLTAGE_THRESHOLD;
  bool reverseState = reverseVolt > VOLTAGE_THRESHOLD;

  // Combined indicator state: both indicators are “on” simultaneously.
  // bool combinedIndicatorState = leftIndicatorState && rightIndicatorState;

  unsigned long currentTime = millis();

  unlockSequence->setInputs(accOnState, leftIndicatorState, rightIndicatorState);
  lockSequence->setInputs(accOnState, leftIndicatorState, rightIndicatorState);
  flickSequence->setInputs(accOnState, leftIndicatorState, rightIndicatorState);

  unlockSequence->loop();
  lockSequence->loop();
  flickSequence->loop();

  if (!accOnState)
  {
    // ACC is off -> Process startup flash logic.

    // // Check for a rising edge on the combined indicator state with debounce.
    // if (combinedIndicatorState && !lastCombinedIndicatorState &&
    //     ((currentTime - lastDebounceTime) >= DEBOUNCE_MS))
    // {
    //   // If this is the first flash, record the time.
    //   if (firstFlashTime == 0)
    //   {
    //     firstFlashTime = currentTime;
    //   }

    //   // Count this flash.
    //   flashCounter++;

    //   // Reset debounce timer.
    //   lastDebounceTime = currentTime;
    // }

    // // Update the previous state for the next call.
    // lastCombinedIndicatorState = combinedIndicatorState;

    // // If at least one flash has occurred, activate the startup effect.
    // if (flashCounter >= FLASHES_TO_ACTIVATE_STARTUP && startupEffect->isActive() == false)
    // {
    //   startupEffect->setActive(true);
    //   flashCounter = 0;
    //   firstFlashTime = 0;
    // }

    // // If the startup effect is active and we've received three additional flashes,
    // // then disable the startup effect.
    // if (flashCounter >= FLASHES_TO_DEACTIVATE_STARTUP && startupEffect->isActive() == true)
    // {
    //   startupEffect->setActive(false);
    //   flashCounter = 0;
    //   firstFlashTime = 0;
    // }

    // // If it's been more than 10 seconds since the first detected flash,
    // // reset the flash counter and firstFlashTime.
    // if (firstFlashTime != 0 && (currentTime - firstFlashTime) > FLASH_RESET_MS)
    // {
    //   flashCounter = 0;
    //   firstFlashTime = 0;
    // }

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
