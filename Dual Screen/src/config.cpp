#include "config.h"

#undef LOG_TAG
#define LOG_TAG "CONFIG"
#include <dcr_Logger.h>

Preferences preferences;
bool wifiConnectedAtSomePoint = false;

void initConfig()
{
  preferences.begin("p", false);
  const long bootCount = preferences.getLong("bootCount", 0) + 1;
  preferences.putLong("bootCount", bootCount);
  debugI("Boot %ld", bootCount);
}
