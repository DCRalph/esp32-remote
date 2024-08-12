#include "config.h"

Preferences preferences;
WiFiManager wm;

uint64_t lastInteract;
int autoOffMin;

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
  preferences.begin("esp-box-thing", false);

  long bootCount = preferences.getLong("bootCount", 0);
  bootCount++;
  preferences.putLong("bootCount", bootCount);

  Serial.println("[INFO] [CONFIG] Boot count: " + String(bootCount));

  autoOffMin = preferences.getInt("autoOffMin", 5);

  lastInteract = millis();
}