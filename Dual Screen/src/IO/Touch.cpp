#include "IO/Touch.h"

#include <XPT2046_Touchscreen.h>

#include "config.h"

namespace
{
  XPT2046_Touchscreen ts(TouchPanelPins::kTouchCS);
  bool ready = false;

  // Rough generic XPT2046 values until Settings > Touch cal runs.
  constexpr int16_t kRawMin = 300;
  constexpr int16_t kRawMax = 3800;
  Touch::Calibration cal{kRawMin, kRawMax, kRawMin, kRawMax};

  TouchInput::Config inputConfig()
  {
    TouchInput::Config c;
    c.read = Touch::read;
    return c;
  }
}

TouchInput Touch::input(inputConfig());

void Touch::begin(SPIClass &bus)
{
  ts.begin(bus);
  ts.setRotation(1);
  cal.xMin = preferences.getShort("t2_xMin", kRawMin);
  cal.xMax = preferences.getShort("t2_xMax", kRawMax);
  cal.yMin = preferences.getShort("t2_yMin", kRawMin);
  cal.yMax = preferences.getShort("t2_yMax", kRawMax);
  ready = true;
}

bool Touch::readRaw(int16_t &x, int16_t &y)
{
  if (!ready || !ts.touched())
    return false;
  const TS_Point p = ts.getPoint();
  x = p.x;
  y = p.y;
  return true;
}

bool Touch::read(int16_t &x, int16_t &y)
{
  int16_t rx, ry;
  if (!readRaw(rx, ry))
    return false;
  x = constrain(map(rx, cal.xMin, cal.xMax, 0, TOUCH_WIDTH - 1), 0, TOUCH_WIDTH - 1);
  y = constrain(map(ry, cal.yMin, cal.yMax, 0, TOUCH_HEIGHT - 1), 0, TOUCH_HEIGHT - 1);
  return true;
}

Touch::Calibration Touch::calibration()
{
  return cal;
}

void Touch::setCalibration(const Calibration &c)
{
  cal = c;
}

void Touch::save()
{
  preferences.putShort("t2_xMin", cal.xMin);
  preferences.putShort("t2_xMax", cal.xMax);
  preferences.putShort("t2_yMin", cal.yMin);
  preferences.putShort("t2_yMax", cal.yMax);
}

void Touch::reset()
{
  cal = {kRawMin, kRawMax, kRawMin, kRawMax};
  save();
}
