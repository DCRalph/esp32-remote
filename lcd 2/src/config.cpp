#include "config.h"

Preferences preferences;
bool wifiConnectedAtSomePoint = false;

bool carRelays[RemoteProtocol::CarRelays::kRelayCount] = {};

void applyCarRelays(const RemoteProtocol::CarRelays::Relays &relays)
{
  for (uint8_t i = 0; i < RemoteProtocol::CarRelays::kRelayCount; i++)
    carRelays[i] = relays.mask & (1u << i);
}

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