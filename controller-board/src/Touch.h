// XPT2046 touch through TFT_eSPI (it shares the panel's SPI bus), with the
// bioreactor's filtering. Calibration: this board's own, saved in NVS by the
// calibrator (screens/TouchCal.cpp); else the bioreactor's file; else generic.
#pragma once

#include <InputSources.h>
#include <TFT_eSPI.h>

namespace Touch
{
  // Raw readings at the screen's edges, after an optional x/y swap. Min above
  // max inverts that axis.
  struct Calibration
  {
    bool swapXY = false;
    int16_t xMin = 300, xMax = 3800, yMin = 300, yMax = 3800;
  };

  enum class Source : uint8_t
  {
    Saved,      // calibrated on this board
    Bioreactor, // the bioreactor's TFT_eSPI calibration file
    Generic,    // neither: rough XPT2046 values
  };

  /** Loads the saved calibration, or falls back as above. */
  void begin(TFT_eSPI &tft);
  Source source();

  /** Raw controller x, y while pressed past the start pressure. */
  bool readRaw(int16_t &x, int16_t &y);
  /** Uses `c` from now on and saves it to NVS, tied to the current rotation. */
  void setCalibration(const Calibration &c);

  struct Reading
  {
    bool touched = false;
    uint16_t rawX = 0, rawY = 0, z = 0;
    int16_t x = 0, y = 0; // panel pixels
  };
  /** One unfiltered read, for the touch test and calibrator screens. */
  Reading read();

  // Synthetic touch for serial control, read by `input` in place of the panel, so
  // it goes through the same filtering and gestures as a finger.
  void injectTap(int16_t x, int16_t y);
  void injectPress(int16_t x, int16_t y); // held until injectRelease()
  void injectRelease();

  extern TouchInput input;
}
