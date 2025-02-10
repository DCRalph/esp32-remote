
#include "secrets.h"

#include "config.h"

#include "IO/Wireless.h"

#include "FastLED.h"

#include "IO/LED/LEDManager.h"
#include "IO/LED/Effects/BrakeLightEffect.h"
#include "IO/LED/Effects/IndicatorEffect.h"

#define NUM_LEDS 120 // Example LED strip length

CRGB leds[NUM_LEDS];

class CustomLEDManager : public LEDManager
{
public:
  CustomLEDManager(uint16_t numLEDs) : LEDManager(numLEDs) {}

  virtual void draw() override
  {
    // Serial.print("LED Buffer: ");
    for (int i = 0; i < numLEDs; i++)
    {
      leds[i] = CRGB(ledBuffer[i].r, ledBuffer[i].g, ledBuffer[i].b);

    }
    FastLED.show();
    // Serial.println();
  }
};

CustomLEDManager *ledManager;

// Global effect pointers.
BrakeLightEffect *brakeEffect;
IndicatorEffect *leftIndicator;
IndicatorEffect *rightIndicator;

// Global simulation control flag
bool simulateEffects = true;

// Other globals (already defined in your code)
unsigned long lastSimTime = 0;
unsigned long simInterval = 4000;
bool brakePressed = false;
bool leftIndicatorActive = false;
bool rightIndicatorActive = false;

void espNowRecvCb(fullPacket *fp)
{
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();

  wireless.setup();
  wireless.setRecvCb(espNowRecvCb);

  FastLED.addLeds<WS2812, 5, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);

  leds[0] = CRGB(255, 0, 0);
  FastLED.show();
  delay(500);
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();

  ledManager = new CustomLEDManager(NUM_LEDS);

  // Create the effect instances.
  brakeEffect = new BrakeLightEffect(NUM_LEDS, 0, false);
  leftIndicator = new IndicatorEffect(NUM_LEDS, IndicatorEffect::LEFT, 1, false);
  rightIndicator = new IndicatorEffect(NUM_LEDS, IndicatorEffect::RIGHT, 1, false);

  ledManager->setFPS(100); // Set the desired FPS for the LED manager.

  // Add the effects to the LED manager.
  ledManager->addEffect(brakeEffect);
  ledManager->addEffect(leftIndicator);
  ledManager->addEffect(rightIndicator);

  led.On();
  delay(500);
  led.Off();
}
void loop() {
  unsigned long currentTime = millis();

  // If simulation mode is enabled, automatically toggle effect states every simInterval.
  if (simulateEffects && (currentTime - lastSimTime > simInterval)) {
    lastSimTime = currentTime;

    // Toggle brake state.
    brakePressed = !brakePressed;
    brakeEffect->setBrakeActive(brakePressed);
    Serial.print("Simulation - Brake Pressed: ");
    Serial.println(brakePressed ? "YES" : "NO");

    // Toggle indicator states.
    leftIndicatorActive = !leftIndicatorActive;
    rightIndicatorActive = !rightIndicatorActive;
    leftIndicator->setIndicatorActive(leftIndicatorActive);
    rightIndicator->setIndicatorActive(rightIndicatorActive);

    Serial.print("Simulation - Left Indicator: ");
    Serial.println(leftIndicatorActive ? "ON" : "OFF");
    Serial.print("Simulation - Right Indicator: ");
    Serial.println(rightIndicatorActive ? "ON" : "OFF");
  }

  // Update and draw the LED effects.
  ledManager->updateEffects();
  ledManager->draw();

  // Process serial commands if available.
  if (Serial.available() > 0) {
    String input = "";
    // Read character by character until no more data.
    while (Serial.available() > 0) {
      char c = Serial.read();
      if (c != '\n' && c != '\r') {  // Skip newline characters.
        input += c;
      }
      delay(2);  // Brief delay between characters.
    }
    input.trim(); // Remove any surrounding whitespace.

    // Parse serial commands.
    if (input.equalsIgnoreCase("reboot")) {
      ESP.restart();
    }
    else if (input.equalsIgnoreCase("getip")) {
      Serial.println(WiFi.localIP());
    }
    else if (input.equalsIgnoreCase("getmac")) {
      Serial.println(WiFi.macAddress());
    }
    else if (input.equalsIgnoreCase("help")) {
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
      Serial.println(F("  sim on      - Enable simulation mode"));
      Serial.println(F("  sim off     - Disable simulation mode"));
      Serial.println(F("  status      - Display current effect states"));
    }
    else if (input.equalsIgnoreCase("b on")) {
      brakePressed = true;
      brakeEffect->setBrakeActive(true);
      Serial.println(F("Brake effect activated."));
    }
    else if (input.equalsIgnoreCase("b off")) {
      brakePressed = false;
      brakeEffect->setBrakeActive(false);
      Serial.println(F("Brake effect deactivated."));
    }
    else if (input.equalsIgnoreCase("l on")) {
      leftIndicatorActive = true;
      leftIndicator->setIndicatorActive(true);
      Serial.println(F("Left indicator activated."));
    }
    else if (input.equalsIgnoreCase("l off")) {
      leftIndicatorActive = false;
      leftIndicator->setIndicatorActive(false);
      Serial.println(F("Left indicator deactivated."));
    }
    else if (input.equalsIgnoreCase("r on")) {
      rightIndicatorActive = true;
      rightIndicator->setIndicatorActive(true);
      Serial.println(F("Right indicator activated."));
    }
    else if (input.equalsIgnoreCase("r off")) {
      rightIndicatorActive = false;
      rightIndicator->setIndicatorActive(false);
      Serial.println(F("Right indicator deactivated."));
    }
    else if (input.equalsIgnoreCase("sim on")) {
      simulateEffects = true;
      Serial.println(F("Simulation mode enabled."));
    }
    else if (input.equalsIgnoreCase("sim off")) {
      simulateEffects = false;
      Serial.println(F("Simulation mode disabled."));
    }
    else if (input.equalsIgnoreCase("status")) {
      Serial.print(F("Brake: "));
      Serial.println(brakePressed ? F("ON") : F("OFF"));
      Serial.print(F("Left Indicator: "));
      Serial.println(leftIndicatorActive ? F("ON") : F("OFF"));
      Serial.print(F("Right Indicator: "));
      Serial.println(rightIndicatorActive ? F("ON") : F("OFF"));
      Serial.print(F("Simulation: "));
      Serial.println(simulateEffects ? F("ENABLED") : F("DISABLED"));
    }
    else {
      Serial.println("[INFO] [SERIAL] Unknown command: " + input);
    }
  }
}