// XPT2046 resistive touch on the ILI9341 panel, with a calibration kept in NVS.
#pragma once

#include <InputSources.h>
#include <SPI.h>

namespace Touch
{
  // Raw XPT2046 readings (after rotation) at the panel's edges.
  struct Calibration
  {
    int16_t xMin, xMax, yMin, yMax;
  };

  /** Starts the controller on the panel's SPI bus and loads the saved calibration. */
  void begin(SPIClass &bus);

  bool readRaw(int16_t &x, int16_t &y);
  /** Panel pixels, through the calibration. */
  bool read(int16_t &x, int16_t &y);

  Calibration calibration();
  void setCalibration(const Calibration &c);
  void save();
  void reset();

  /** Register on the touch display: `touchDisplay.addInput(Touch::input)`. */
  extern TouchInput input;
}
