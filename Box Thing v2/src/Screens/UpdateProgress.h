#pragma once

#include "config.h"
#include "Screens/Screens.h"

/** Full-screen OTA progress readout. Driven by the ArduinoOTA callbacks. */
namespace UpdateProgress
{
  void setProgress(uint8_t progress);
  void setMessage(String message);
}
