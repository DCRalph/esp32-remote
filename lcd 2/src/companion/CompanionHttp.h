#pragma once

#include "CompanionProtocol.h"

enum class CompanionPairState {
  WifiDown,
  Discovering,
  PendingApproval,
  Ready,
};

class CompanionHttp {
public:
  static void begin();
  static void shutdown();
  static bool isActive();
  static void loop();

  static CompanionProtocol &protocol();
  static CompanionPairState getPairState();
  static bool isConnected();
  static const char *getDeviceName();
  static const char *getMacString();
  static void restartPairing();

  static void sendPageNext();
  static void sendPagePrev();
  static void sendWaveformRefresh();

  static bool ensureBaseUrl();

private:
  static bool registerClient();
  static bool pollOnce();
};
