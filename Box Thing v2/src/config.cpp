#include "config.h"

Preferences preferences;
WiFiManager wm;

uint64_t lastInteract;
int autoOffMin;

uint32_t fps = 0;
uint32_t lastFps = 0;
uint32_t frameTime = 0;
uint32_t lastFrameTime = 0;

uint64_t clearBufferTime = 0;
uint64_t screenManagerDrawTime = 0;
uint64_t drawTopBarTime = 0;
uint64_t sendBufferTime = 0;
uint64_t screenUpdateDrawTime = 0;

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