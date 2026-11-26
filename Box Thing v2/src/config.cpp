#include "config.h"

#undef LOG_TAG
#define LOG_TAG "CONFIG"

Preferences preferences;

Wireless &wireless = *Wireless::getInstance();

bool wifiConnectedAtSomePoint = false;

int autoOffMin;

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

}
