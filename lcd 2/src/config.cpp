#include "config.h"

Preferences preferences;

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