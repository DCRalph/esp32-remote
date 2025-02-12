// application.cpp

#include "Application.h"
#include "config.h"      // Contains NUM_LEDS, LEDS_PIN, etc.
#include "IO/Wireless.h" // Your wireless library
#include "FastLED.h"

//----------------------------------------------------------------------------
/*
 * CustomLEDManager::draw()
 * Copy the led buffer to the FastLED object and show the changes.
 */
void CustomLEDManager::draw()
{
  for (int i = 0; i < numLEDs; i++)
  {
    Application::getInstance()->leds[i] =
        CRGB(ledBuffer[i].r, ledBuffer[i].g, ledBuffer[i].b);
  }
  FastLED.show();
}

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
  ledManager = new CustomLEDManager(NUM_LEDS);
  ledManager->setFPS(100);

  // Set each effect's LED manager pointer.
  brakeEffect = new BrakeLightEffect(ledManager, 5, false);
  leftIndicatorEffect = new IndicatorEffect(ledManager, IndicatorEffect::LEFT,
                                            10, true);
  rightIndicatorEffect = new IndicatorEffect(ledManager, IndicatorEffect::RIGHT,
                                             10, true);
  reverseLightEffect = new ReverseLightEffect(ledManager, 6, false);
  rgbEffect = new RGBEffect(ledManager, 0, false);
  startupEffect = new StartupEffect(ledManager, 9, false);

  leftIndicatorEffect->setOtherIndicator(rightIndicatorEffect);
  rightIndicatorEffect->setOtherIndicator(leftIndicatorEffect);


  // Add effects to the LED manager.
  ledManager->addEffect(brakeEffect);
  ledManager->addEffect(leftIndicatorEffect);
  ledManager->addEffect(rightIndicatorEffect);
  ledManager->addEffect(reverseLightEffect);
  ledManager->addEffect(rgbEffect);
  ledManager->addEffect(startupEffect);
}

/*
 * update():
 * Main loop update.
 */
void Application::update()
{

  // If in test mode, skip input handling and use defaults.
  if (testMode)
  {
    // (Optional) You could cycle effects in test mode here.
    // For this example we don't change effect states.
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
void Application::handleEffects()
{
  // Read physical inputs. Note: Assuming the read() method returns a bool.
  bool accOnState = accOn->analogRead() > analogThreshold;
  bool brakeState = brake->analogRead() > analogThreshold;
  bool leftIndicatorState = leftIndicator->analogRead() > analogThreshold;
  bool rightIndicatorState = rightIndicator->analogRead() > analogThreshold;
  bool reverseState = reverse->analogRead() > analogThreshold;

  // When ACC is off.
  if (!accOnState)
  {
    // The startup effect is triggered if both indicators are active.
    if (leftIndicatorState && rightIndicatorState)
    {
      startupEffect->setActive(true);
    }

    // When ACC is off, ignore other effects.
    brakeEffect->setActive(false);
    leftIndicatorEffect->setIndicatorActive(false);
    rightIndicatorEffect->setIndicatorActive(false);
    reverseLightEffect->setActive(false);
  }
  else
  {
    // ACC is on, so disable the startup effect if it was active.
    if (startupEffect->isActive())
    {
      startupEffect->setActive(false);
    }

    // Brake effect: activate only if brake is pressed.
    brakeEffect->setActive(brakeState);
    brakeEffect->setIsReversing(reverseState);

    // Indicator effects: each one on if its respective input is active.
    leftIndicatorEffect->setIndicatorActive(leftIndicatorState);
    rightIndicatorEffect->setIndicatorActive(rightIndicatorState);

    // Reverse light effect: active if reverse is on.
    reverseLightEffect->setActive(reverseState);
  }
}
