#pragma once

#include <dcr_display.h>

extern const Screen2 HomeScreen;
extern const Screen2 TouchTestScreen;
extern const Screen2 DeviceScreen;

namespace App
{
  /** Asks loop() to run touch calibration between frames (it draws straight to the panel). */
  void requestCalibration();
}
