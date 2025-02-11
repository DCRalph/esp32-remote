// #include "secrets.h"
#include "config.h"
#include "IO/Wireless.h"
#include "FastLED.h"
#include "IO/LED/LEDManager.h"
#include "IO/LED/Effects/BrakeLightEffect.h"
#include "IO/LED/Effects/IndicatorEffect.h"
#include "IO/LED/Effects/ReverseLightEffect.h"
#include "IO/LED/Effects/RGBEffect.h"
#include "IO/LED/Effects/StartupEffect.h"



CRGB leds[NUM_LEDS];

class CustomLEDManager : public LEDManager
{
public:
  CustomLEDManager(uint16_t numLEDs) : LEDManager(numLEDs) {}

  virtual void draw() override
  {
    for (int i = 0; i < numLEDs; i++)
    {
      leds[i] = CRGB(ledBuffer[i].r, ledBuffer[i].g, ledBuffer[i].b);
    }
    FastLED.show();
  }
};

CustomLEDManager *ledManager;

// Global effect pointers.
BrakeLightEffect *brakeEffect;
IndicatorEffect *leftIndicator;
IndicatorEffect *rightIndicator;
ReverseLightEffect *reverseLight;
RGBEffect *rgbEffect;
StartupEffect *startupEffect;

// Global simulation control flag
bool simulateEffects = false;

// Other globals (already defined in your code)
unsigned long lastSimTime = 0;
unsigned long simInterval = 5000;
bool brakePressed = false;
bool leftIndicatorActive = false;
bool rightIndicatorActive = false;
bool reverseLightActive = false;
bool rgbEffectActive = false;
bool startupEffectActive = false;

// Wireless timeout (ms)
const unsigned long WIRELESS_TIMEOUT_MS = 5000;
// Last time a wireless packet was received.
unsigned long lastRecvTime = 0;

// Packet types
enum PacketType
{
  CAR_CONTROL_PACKET = 0,
  EFFECT_CONTROL_PACKET = 1
};

// Structure for car control data
struct CarControlData
{
  bool brake;
  bool leftIndicator;
  bool rightIndicator;
  bool reverseLight;
};

// Structure for effect control data
struct EffectControlData
{
  bool rgbEffect;
  bool startupEffect;
};

// Function to deactivate all effects
void deactivateAllEffects()
{
  brakeEffect->setBrakeActive(false);
  leftIndicator->setIndicatorActive(false);
  rightIndicator->setIndicatorActive(false);
  reverseLight->setActive(false);
  rgbEffect->setActive(false);
  startupEffect->setActive(false);

  brakePressed = false;
  leftIndicatorActive = false;
  rightIndicatorActive = false;
  reverseLightActive = false;
  rgbEffectActive = false;
  startupEffectActive = false;
}

// ESP-NOW receive callback function
void espNowRecvCb(fullPacket *fp)
{
  lastRecvTime = millis(); // Update last receive time

  // Check packet type and length
  if (fp->p.type == CAR_CONTROL_PACKET && fp->p.len == sizeof(CarControlData))
  {
    CarControlData carData;
    memcpy(&carData, fp->p.data, sizeof(CarControlData));

    brakeEffect->setBrakeActive(carData.brake);
    leftIndicator->setIndicatorActive(carData.leftIndicator);
    rightIndicator->setIndicatorActive(carData.rightIndicator);
    reverseLight->setActive(carData.reverseLight);

    brakePressed = carData.brake;
    leftIndicatorActive = carData.leftIndicator;
    rightIndicatorActive = carData.rightIndicator;
    reverseLightActive = carData.reverseLight;
  }
  else if (fp->p.type == EFFECT_CONTROL_PACKET && fp->p.len == sizeof(EffectControlData))
  {
    EffectControlData effectData;
    memcpy(&effectData, fp->p.data, sizeof(EffectControlData));

    rgbEffect->setActive(effectData.rgbEffect);
    startupEffect->setActive(effectData.startupEffect);

    rgbEffectActive = effectData.rgbEffect;
    startupEffectActive = effectData.startupEffect;
  }
  else
  {
    Serial.println("[ERROR] [WIRELESS] Invalid packet received!");
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();

  wireless.setup();
  wireless.setRecvCb(espNowRecvCb);

  FastLED.addLeds<WS2812, LEDS_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);

  leds[0] = CRGB(255, 0, 0);
  FastLED.show();
  delay(500);
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();

  ledManager = new CustomLEDManager(NUM_LEDS);
  ledManager->setFPS(100);

  // Create the effect instances.
  brakeEffect = new BrakeLightEffect(ledManager, 5, false);
  leftIndicator = new IndicatorEffect(ledManager, IndicatorEffect::LEFT, 10, true);
  rightIndicator = new IndicatorEffect(ledManager, IndicatorEffect::RIGHT, 10, true);
  reverseLight = new ReverseLightEffect(ledManager, 6, false);
  rgbEffect = new RGBEffect(ledManager, 0, false);
  startupEffect = new StartupEffect(ledManager, 9, false);

  // Add the effects to the LED manager.
  ledManager->addEffect(brakeEffect);
  ledManager->addEffect(leftIndicator);
  ledManager->addEffect(rightIndicator);
  ledManager->addEffect(reverseLight);
  ledManager->addEffect(rgbEffect);
  ledManager->addEffect(startupEffect);

  // startupEffect->setActive(true);
  // rgbEffect->setActive(true);

  lastRecvTime = millis(); // Initialize last receive time

  led.On();
  delay(500);
  led.Off();
}

void loop()
{
  unsigned long currentTime = millis();
  wireless.loop();

  // Check for wireless timeout
  // if (currentTime - lastRecvTime > WIRELESS_TIMEOUT_MS)
  // {
  //   Serial.println("[WARNING] [WIRELESS] Connection lost!");
  //   deactivateAllEffects();
  // }

  // If simulation mode is enabled, automatically toggle effect states every simInterval.
  if (simulateEffects && (currentTime - lastSimTime > simInterval))
  {
    lastSimTime = currentTime;

    // Toggle indicator states.
    // leftIndicatorActive = !leftIndicatorActive;
    // rightIndicatorActive = !rightIndicatorActive;
    // leftIndicator->setIndicatorActive(leftIndicatorActive);
    // rightIndicator->setIndicatorActive(rightIndicatorActive);
  }

  brakeEffect->setBrakeActive(io0.read());
  // leftIndicator->setIndicatorActive(io0.read());
  // rightIndicator->setIndicatorActive(io0.read());
  // reverseLight->setActive(io0.read());
  // rgbEffect->setActive(io0.read());
  // startupEffect->setActive(io0.read());

  // Update and draw the LED effects.
  ledManager->updateEffects();
  ledManager->draw();

  // Process serial commands if available.
  if (Serial.available() > 0)
  {
    String input = "";
    // Read character by character until no more data.
    while (Serial.available() > 0)
    {
      char c = Serial.read();
      if (c != '\n' && c != '\r')
      { // Skip newline characters.
        input += c;
      }
      delay(2); // Brief delay between characters.
    }
    input.trim(); // Remove any surrounding whitespace.

    // Parse serial commands.
    if (input.equalsIgnoreCase("reboot"))
    {
      ESP.restart();
    }
    else if (input.equalsIgnoreCase("getip"))
    {
      Serial.println(WiFi.localIP());
    }
    else if (input.equalsIgnoreCase("getmac"))
    {
      Serial.println(WiFi.macAddress());
    }
    else if (input.equalsIgnoreCase("help"))
    {
      Serial.println(F("Available commands:"));
      Serial.println(F("  reboot      - Restart the device"));
      Serial.println(F("  getip       - Get the WiFi IP address"));
      Serial.println(F("  getmac      - Get the MAC address"));
      Serial.println(F("  b on    - Activate brake effect"));
      Serial.println(F("  b off   - Deactivate brake effect"));
      Serial.println(F("  l on     - Activate left indicator"));
      Serial.println(F("  l off    - Deactivate left indicator"));
      Serial.println(F("  r on    - Activate right indicator"));
      Serial.println(F("  r off   - Deactivate right indicator"));
      Serial.println(F("  rev on  - Activate reverse light"));
      Serial.println(F("  rev off - Deactivate reverse light"));
      Serial.println(F("  rgb on  - Activate RGB effect"));
      Serial.println(F("  rgb off - Deactivate RGB effect"));
      Serial.println(F("  sim on      - Enable simulation mode"));
      Serial.println(F("  sim off     - Disable simulation mode"));
      Serial.println(F("  status      - Display current effect states"));
    }
    else if (input.equalsIgnoreCase("b on"))
    {
      brakePressed = true;
      brakeEffect->setBrakeActive(true);
      Serial.println(F("Brake effect activated."));
    }
    else if (input.equalsIgnoreCase("b off"))
    {
      brakePressed = false;
      brakeEffect->setBrakeActive(false);
      Serial.println(F("Brake effect deactivated."));
    }
    else if (input.equalsIgnoreCase("l on"))
    {
      leftIndicatorActive = true;
      leftIndicator->setIndicatorActive(true);
      Serial.println(F("Left indicator activated."));
    }
    else if (input.equalsIgnoreCase("l off"))
    {
      leftIndicatorActive = false;
      leftIndicator->setIndicatorActive(false);
      Serial.println(F("Left indicator deactivated."));
    }
    else if (input.equalsIgnoreCase("r on"))
    {
      rightIndicatorActive = true;
      rightIndicator->setIndicatorActive(true);
      Serial.println(F("Right indicator activated."));
    }
    else if (input.equalsIgnoreCase("r off"))
    {
      rightIndicatorActive = false;
      rightIndicator->setIndicatorActive(false);
      Serial.println(F("Right indicator deactivated."));
    }
    else if (input.equalsIgnoreCase("rev on"))
    {
      reverseLightActive = true;
      reverseLight->setActive(true);
      Serial.println(F("Reverse light activated."));
    }
    else if (input.equalsIgnoreCase("rev off"))
    {
      reverseLightActive = false;
      reverseLight->setActive(false);
      Serial.println(F("Reverse light deactivated."));
    }
    else if (input.equalsIgnoreCase("rgb on"))
    {
      rgbEffectActive = true;
      rgbEffect->setActive(true);
      Serial.println(F("RGB effect activated."));
    }
    else if (input.equalsIgnoreCase("rgb off"))
    {
      rgbEffectActive = false;
      rgbEffect->setActive(false);
      Serial.println(F("RGB effect deactivated."));
    }
    else if (input.equalsIgnoreCase("sim on"))
    {
      simulateEffects = true;
      Serial.println(F("Simulation mode enabled."));
    }
    else if (input.equalsIgnoreCase("sim off"))
    {
      simulateEffects = false;
      Serial.println(F("Simulation mode disabled."));
    }
    else if (input.equalsIgnoreCase("status"))
    {
      Serial.print(F("Brake: "));
      Serial.println(brakePressed ? F("ON") : F("OFF"));
      Serial.print(F("Left Indicator: "));
      Serial.println(leftIndicatorActive ? F("ON") : F("OFF"));
      Serial.print(F("Right Indicator: "));
      Serial.println(rightIndicatorActive ? F("ON") : F("OFF"));
      Serial.print(F("Reverse Light: "));
      Serial.println(reverseLightActive ? F("ON") : F("OFF"));
      Serial.print(F("RGB Effect: "));
      Serial.println(rgbEffectActive ? F("ON") : F("OFF"));
      Serial.print(F("Simulation: "));
      Serial.println(simulateEffects ? F("ENABLED") : F("DISABLED"));
    }
    else
    {
      Serial.println("[INFO] [SERIAL] Unknown command: " + input);
    }
  }
}
