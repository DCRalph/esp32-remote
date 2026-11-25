// What this board needs before the UI starts: the power latch, the backlight, and
// facts the bioreactor firmware stored. Device data is only ever read, never written.
#pragma once

#include <Arduino.h>

namespace Device
{
  struct Info
  {
    bool found = false;      // the bioreactor's "deviceInfo" blob was in NVS
    int hardwareVersion = 0; // picks the backlight polarity
    int deviceType = 0;      // bioreactor DeviceType; the 8-way unit mounts the panel flipped
    char serial[9] = "";
    uint8_t rotation = 1;    // TFT_eSPI rotation for this unit
    bool backlightActiveHigh = true;
  };

  /** Holds the power latch, then reads NVS. Call first in setup(). */
  void begin();
  const Info &info();

  void setBacklight(bool on);

  /**
   * The bioreactor's saved TFT_eSPI touch calibration (5 values), from LittleFS.
   * The filesystem is mounted without format-on-fail and unmounted again.
   */
  bool loadTouchCalibration(uint16_t (&calData)[5]);

  /** Polls the power button; a long hold switches the board off. */
  void loop();
  void powerOff();
}
