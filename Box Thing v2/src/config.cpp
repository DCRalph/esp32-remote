#include "config.h"

#undef LOG_TAG
#define LOG_TAG "CONFIG"

Preferences preferences;

Wireless &wireless = *Wireless::getInstance();

bool wifiConnectedAtSomePoint = false;

uint64_t lastInteract;
int autoOffMin;

uint32_t fps = 0;
uint32_t lastFps = 0;
uint32_t frameTime = 0;
uint32_t lastFrameTime = 0;

void initConfig()
{
  debugI("Starting...");

  debugI("Preferences...");
  preferences.begin("esp-box-thing", false);

  long bootCount = preferences.getLong("bootCount", 0);
  bootCount++;
  preferences.putLong("bootCount", bootCount);

  debugI("Boot count: %ld", bootCount);

  autoOffMin = preferences.getInt("autoOffMin", 5);

  lastInteract = millis();
}
