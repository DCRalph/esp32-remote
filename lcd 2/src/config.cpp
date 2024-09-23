#include "config.h"

Preferences preferences;

bool globalRelay1 = false;
bool globalRelay2 = false;
bool globalRelay3 = false;
bool globalRelay4 = false;
bool globalRelay5 = false;
bool globalRelay6 = false;

void initConfig()
{
  Serial.begin(BAUD_RATE);
  Serial.println("[INFO] [CONFIG] Starting...");

  Serial.println("[INFO] [CONFIG] Preferences...");
  preferences.begin("p", false);

  long bootCount = preferences.getLong("bootCount", 0);
  bootCount++;
  preferences.putLong("bootCount", bootCount);
}