#include "Touch.h"

#include <Preferences.h>

#include "Device.h"
#include "config.h"

#undef LOG_TAG
#define LOG_TAG "TOUCH"
#include <dcr_Logger.h>

namespace
{
  TFT_eSPI *panel = nullptr;
  Touch::Source calSource = Touch::Source::Generic;
  Touch::Calibration cal;
  // The bioreactor's TFT_eSPI calibration: x0, x1, y0, y1, orientation flags.
  uint16_t bioreactorCal[5] = {};

  // This board's own NVS namespace; the bioreactor's "prefs" is never written.
  constexpr char kNamespace[] = "controller";
  constexpr char kCalKey[] = "touchCal";
  constexpr uint8_t kCalVersion = 1;

  struct StoredCal
  {
    uint8_t version;
    uint8_t rotation; // the calibration only holds at the rotation it was taken in
    Touch::Calibration cal;
  };

  enum class Synthetic : uint8_t
  {
    None,
    Tap,
    Held,
  };
  Synthetic synthetic = Synthetic::None;
  int16_t syntheticX = 0, syntheticY = 0;
  uint8_t tapPollsLeft = 0;
  // Polls a tap stays down: about two frames (input polls at render and mid-frame).
  constexpr uint8_t kTapPolls = 4;
  constexpr uint16_t kSyntheticPressure = 2000;

  bool loadSaved()
  {
    Preferences prefs;
    if (!prefs.begin(kNamespace, true))
      return false;
    StoredCal stored{};
    const bool ok = prefs.getBytesLength(kCalKey) == sizeof(stored) &&
                    prefs.getBytes(kCalKey, &stored, sizeof(stored)) == sizeof(stored) &&
                    stored.version == kCalVersion && stored.rotation == Device::info().rotation;
    prefs.end();
    if (ok)
      cal = stored.cal;
    return ok;
  }

  // Raw controller reading to panel pixels, unclamped so readings off the panel
  // reach TouchInput's bounds check as noise.
  void toPanel(uint16_t rawX, uint16_t rawY, int16_t &x, int16_t &y)
  {
    if (calSource == Touch::Source::Bioreactor)
    {
      uint16_t px = rawX, py = rawY;
      panel->convertRawXY(&px, &py);
      x = static_cast<int16_t>(px);
      y = static_cast<int16_t>(py);
      return;
    }
    const int32_t a = cal.swapXY ? rawY : rawX;
    const int32_t b = cal.swapXY ? rawX : rawY;
    x = static_cast<int16_t>(map(a, cal.xMin, cal.xMax, 0, LCD_WIDTH - 1));
    y = static_cast<int16_t>(map(b, cal.yMin, cal.yMax, 0, LCD_HEIGHT - 1));
  }

  // Raw z first so an idle panel costs one SPI transaction. Pressure goes to
  // TouchInput, which applies the start/hold thresholds.
  bool readSample(TouchSample &s)
  {
    if (synthetic == Synthetic::Tap && tapPollsLeft > 0 && --tapPollsLeft == 0)
      synthetic = Synthetic::None;
    if (synthetic != Synthetic::None)
    {
      s.x = syntheticX;
      s.y = syntheticY;
      s.z = kSyntheticPressure;
      return true;
    }
    if (panel == nullptr)
      return false;
    const uint16_t z = panel->getTouchRawZ();
    if (z <= TouchFilter::kHoldPressure)
      return false;
    uint16_t x = 0, y = 0;
    panel->getTouchRaw(&x, &y);
    toPanel(x, y, s.x, s.y);
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
  if (loadSaved())
    calSource = Source::Saved;
  else if (Device::loadTouchCalibration(bioreactorCal))
  {
    calSource = Source::Bioreactor;
    // The file maps raw readings at the bioreactor's rotation; turned 180
    // degrees, both screen axes run the other way (TFT_eSPI invert x and y flags).
    if (ROTATE_180)
      bioreactorCal[4] ^= 0x06;
    tft.setTouch(bioreactorCal);
  }
  else
    calSource = Source::Generic;
  debugI("Touch calibration: %s", calSource == Source::Saved ? "saved" : calSource == Source::Bioreactor ? "bioreactor" : "generic");
}

Touch::Source Touch::source()
{
  return calSource;
}

bool Touch::readRaw(int16_t &x, int16_t &y)
{
  if (panel == nullptr || panel->getTouchRawZ() <= TouchFilter::kStartPressure)
    return false;
  uint16_t rx = 0, ry = 0;
  panel->getTouchRaw(&rx, &ry);
  x = static_cast<int16_t>(rx);
  y = static_cast<int16_t>(ry);
  return true;
}

void Touch::setCalibration(const Calibration &c)
{
  cal = c;
  calSource = Source::Saved;
  Preferences prefs;
  if (!prefs.begin(kNamespace, false))
  {
    debugW("Could not open NVS; calibration kept until restart");
    return;
  }
  const StoredCal stored{kCalVersion, Device::info().rotation, c};
  prefs.putBytes(kCalKey, &stored, sizeof(stored));
  prefs.end();
  debugI("Touch calibration saved: swap %d x %d..%d y %d..%d", c.swapXY, c.xMin, c.xMax, c.yMin, c.yMax);
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
    toPanel(r.rawX, r.rawY, r.x, r.y);
  }
  return r;
}

void Touch::injectTap(int16_t x, int16_t y)
{
  syntheticX = x;
  syntheticY = y;
  tapPollsLeft = kTapPolls + 1; // the first poll consumes one before reporting
  synthetic = Synthetic::Tap;
}

void Touch::injectPress(int16_t x, int16_t y)
{
  syntheticX = x;
  syntheticY = y;
  synthetic = Synthetic::Held;
}

void Touch::injectRelease()
{
  synthetic = Synthetic::None;
}
