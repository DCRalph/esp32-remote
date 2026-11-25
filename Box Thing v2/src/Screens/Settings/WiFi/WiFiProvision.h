// Shared state for the two-step WiFi provisioning flow: pick an SSID on the scan
// screen, then type its password on the password screen.
#pragma once

#include <Arduino.h>

namespace WiFiProvision
{
  inline String pendingSsid;
  inline bool pendingHidden = false;
}
