#include "Touch.h"

#include "Device.h"
#include "config.h"

namespace
{
  TFT_eSPI *panel = nullptr;
  bool savedCalibration = false;

  // Placeholder until Calibrate runs: TFT_eSPI's x0, x1, y0, y1, orientation flags.
  uint16_t calData[5] = {300, 3600, 300, 3600, 1};

  // Raw z first so an idle panel costs one SPI transaction. Pressure goes to
  // TouchInput, which applies the start/hold thresholds.
  bool readSample(TouchSample &s)
  {
    if (panel == nullptr)
      return false;
    const uint16_t z = panel->getTouchRawZ();
    if (z <= TouchFilter::kHoldPressure)
      return false;
    uint16_t x = 0, y = 0;
    panel->getTouchRaw(&x, &y);
    panel->convertRawXY(&x, &y);
    s.x = static_cast<int16_t>(x);
    s.y = static_cast<int16_t>(y);
    s.z = z;
    return true;
  }

  TouchInput::Config inputConfig()
  {
    TouchInput::Config c;
    c.readSample = readSample;
    c.releasePolls = TouchFilter::kReleasePolls;
    c.startPressure = TouchFilter::kStartPressure;
    c.holdPressure = TouchFilter::kHoldPressure;
    c.agreePx = TouchFilter::kAgreePx;
    c.width = LCD_WIDTH;
    c.height = LCD_HEIGHT;
    return c;
  }
}

TouchInput Touch::input(inputConfig());

void Touch::begin(TFT_eSPI &tft)
{
  panel = &tft;
  savedCalibration = Device::loadTouchCalibration(calData);
  tft.setTouch(calData);
}

bool Touch::hasSavedCalibration()
{
  return savedCalibration;
}

void Touch::calibrate(TFT_eSPI &tft)
{
  // Same call and colours as the bioreactor's calibration screen. Draws straight
  // to the panel and blocks until all four corners are touched.
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Touch each corner arrow", LCD_WIDTH / 2, LCD_HEIGHT / 2, 2);
  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
  tft.setTouch(calData);
}

Touch::Reading Touch::read()
{
  Reading r;
  if (panel == nullptr)
    return r;
  r.z = panel->getTouchRawZ();
  r.touched = r.z > TouchFilter::kStartPressure;
  if (r.z > TouchFilter::kHoldPressure)
  {
    panel->getTouchRaw(&r.rawX, &r.rawY);
    r.x = r.rawX;
    r.y = r.rawY;
    panel->convertRawXY(&r.x, &r.y);
  }
  return r;
}
