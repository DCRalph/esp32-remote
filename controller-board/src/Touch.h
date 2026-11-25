// XPT2046 touch through TFT_eSPI (it shares the panel's SPI bus), with the
// bioreactor's calibration and filtering.
#pragma once

#include <InputSources.h>
#include <TFT_eSPI.h>

namespace Touch
{
  /** Applies the bioreactor's saved calibration, or a generic one if there is none. */
  void begin(TFT_eSPI &tft);
  bool hasSavedCalibration();

  /** Runs TFT_eSPI's four-corner calibration on the panel. Kept in RAM only. */
  void calibrate(TFT_eSPI &tft);

  struct Reading
  {
    bool touched = false;
    uint16_t rawX = 0, rawY = 0, z = 0;
    uint16_t x = 0, y = 0; // panel pixels
  };
  /** One unfiltered read, for the touch test screen. */
  Reading read();

  // Synthetic touch for serial control, read by `input` in place of the panel, so
  // it goes through the same filtering and gestures as a finger.
  void injectTap(int16_t x, int16_t y);
  void injectPress(int16_t x, int16_t y); // held until injectRelease()
  void injectRelease();

  extern TouchInput input;
}
