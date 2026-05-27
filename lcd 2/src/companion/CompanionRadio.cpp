#include "CompanionRadio.h"

#include "config.h"

#include <WiFi.h>
#include <dcr_NetLink.h>

#include "Wireless.h"

static bool s_meshSuspended = false;

void CompanionRadio::suspendMeshForCompanion() {
  if (s_meshSuspended) return;

#if DEBUG
  Serial.println("[CompanionRadio] suspending WiFi / ESP-NOW");
#endif

  Wireless::getInstance()->end();
  // Keep WiFi STA up for HTTP companion (netLink).
  if (!netLink.isConnected()) {
    netLink.on();
    netLink.connect();
  }

  s_meshSuspended = true;
}

void CompanionRadio::resumeMeshAfterCompanion() {
  if (!s_meshSuspended) return;

#if DEBUG
  Serial.println("[CompanionRadio] resuming WiFi / ESP-NOW");
#endif

  Wireless::getInstance()->begin();

  s_meshSuspended = false;
}

bool CompanionRadio::isMeshSuspended() { return s_meshSuspended; }
