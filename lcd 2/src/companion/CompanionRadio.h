#pragma once

/** Suspend ESP-NOW / WiFi so BLE companion pairing owns the radio. */
class CompanionRadio {
public:
  static void suspendMeshForCompanion();
  static void resumeMeshAfterCompanion();
  static bool isMeshSuspended();
};
