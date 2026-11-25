// Touch calibration, after Dual Screen's: tap a marker in each corner, then test.
// The result is saved (Touch::setCalibration) and loaded on every boot.
#include "screens/Screens.h"

#include "Touch.h"
#include "config.h"

namespace
{
  constexpr int16_t kInset = 30;
  constexpr int kPoints = 4; // TL, TR, BR, BL
  constexpr int kMinSamples = 5;
  constexpr int32_t kMinSpan = 200; // raw counts across the screen; less is a bad run

  enum class Stage
  {
    Intro,
    Collect,
    Done,
  };

  Stage stage = Stage::Intro;
  int point = 0;
  int32_t sumX = 0, sumY = 0;
  int samples = 0;
  int16_t rawX[kPoints], rawY[kPoints];
  bool failed = false;
  Touch::Reading test;

  Button cancelButton("Cancel", []()
                      { screenManager.back(); });
  Button startButton("Start", []()
                     { stage = Stage::Collect; },
                     Button::Style::Primary);
  Button retryButton("Retry", []()
                     {
                       stage = Stage::Collect;
                       point = samples = 0;
                       sumX = sumY = 0; });
  Button doneButton("Done", []()
                    { screenManager.back(); },
                    Button::Style::Primary);
  FocusGroup introGroup, doneGroup;
  bool built = false;

  void target(int i, int16_t &x, int16_t &y)
  {
    x = (i == 1 || i == 2) ? LCD_WIDTH - 1 - kInset : kInset;
    y = (i >= 2) ? LCD_HEIGHT - 1 - kInset : kInset;
  }

  void text(const String &s, int16_t x, int16_t y, TextRole role, uint16_t color, uint8_t datum = Datum::MiddleCenter)
  {
    display.setTextRole(role);
    display.setTextColor(color);
    display.setTextDatum(datum);
    display.drawString(s, x, y);
  }

  // Which raw axis follows screen x, from how each changes between the left and
  // right markers, then each edge's average extrapolated from the inset targets
  // to the panel edges. A reversed axis comes out with min above max.
  bool computeAndSave()
  {
    const int32_t rawXSpan = abs((rawX[1] + rawX[2]) - (rawX[0] + rawX[3]));
    const int32_t rawYSpan = abs((rawY[1] + rawY[2]) - (rawY[0] + rawY[3]));
    const bool swap = rawYSpan > rawXSpan;
    int32_t a[kPoints], b[kPoints];
    for (int i = 0; i < kPoints; i++)
    {
      a[i] = swap ? rawY[i] : rawX[i];
      b[i] = swap ? rawX[i] : rawY[i];
    }
    const float left = (a[0] + a[3]) / 2.0f, right = (a[1] + a[2]) / 2.0f;
    const float top = (b[0] + b[1]) / 2.0f, bottom = (b[2] + b[3]) / 2.0f;
    if (fabsf(right - left) < kMinSpan || fabsf(bottom - top) < kMinSpan)
      return false;
    const float ppx = (right - left) / ((LCD_WIDTH - 1) - 2 * kInset);
    const float ppy = (bottom - top) / ((LCD_HEIGHT - 1) - 2 * kInset);
    Touch::Calibration c;
    c.swapXY = swap;
    c.xMin = static_cast<int16_t>(lroundf(left - ppx * kInset));
    c.xMax = static_cast<int16_t>(lroundf(right + ppx * kInset));
    c.yMin = static_cast<int16_t>(lroundf(top - ppy * kInset));
    c.yMax = static_cast<int16_t>(lroundf(bottom + ppy * kInset));
    Touch::setCalibration(c);
    return true;
  }

  void enter()
  {
    if (!built)
    {
      built = true;
      const int16_t y = LCD_HEIGHT - 44;
      cancelButton.bounds = {16, y, 132, 34};
      startButton.bounds = {static_cast<int16_t>(LCD_WIDTH - 148), y, 132, 34};
      retryButton.bounds = cancelButton.bounds;
      doneButton.bounds = startButton.bounds;
      introGroup.add(cancelButton);
      introGroup.add(startButton);
      doneGroup.add(retryButton);
      doneGroup.add(doneButton);
    }
    stage = Stage::Intro;
    point = samples = 0;
    sumX = sumY = 0;
    failed = false;
  }

  void draw()
  {
    const Theme &t = display.theme();
    const int16_t cx = LCD_WIDTH / 2;
    switch (stage)
    {
    case Stage::Intro:
      text("Touch calibration", cx, 62, TextRole::Title, t.text);
      text(failed ? "That run was off. Try again:" : "Tap each marker as it appears,", cx, 100, TextRole::Body,
           failed ? t.danger : t.mutedText);
      text("holding still until you lift.", cx, 122, TextRole::Body, t.mutedText);
      text(Touch::source() == Touch::Source::Saved ? "Current: saved on this board" : "Current: not yet calibrated here",
           cx, 154, TextRole::Small, t.mutedText);
      introGroup.draw();
      break;
    case Stage::Collect:
    {
      display.noTopBar();
      int16_t x, y;
      target(point, x, y);
      display.drawSmoothArc(x, y, 12, 9, 0, 0, t.accent, t.background, false);
      display.drawLine(x - 18, y, x + 18, y, t.danger);
      display.drawLine(x, y - 18, x, y + 18, t.danger);
      text(String("Point ") + (point + 1) + " of " + kPoints, cx, LCD_HEIGHT / 2, TextRole::Title, t.text);
      if (samples > 0)
        text("Lift to take it", cx, LCD_HEIGHT / 2 + 28, TextRole::Body, t.mutedText);
      break;
    }
    case Stage::Done:
      text("Saved", cx, 54, TextRole::Title, t.success);
      text("Touch anywhere to test it.", cx, 84, TextRole::Body, t.mutedText);
      if (test.touched)
      {
        display.drawLine(test.x - 10, test.y, test.x + 10, test.y, t.danger);
        display.drawLine(test.x, test.y - 10, test.x, test.y + 10, t.danger);
        text(String(test.x) + ", " + test.y, cx, 112, TextRole::Small, t.mutedText);
      }
      doneGroup.draw();
      break;
    }
  }

  void update()
  {
    if (stage == Stage::Intro)
    {
      introGroup.update();
      return;
    }
    if (stage == Stage::Done)
    {
      test = Touch::read();
      doneGroup.update();
      return;
    }

    // Collect: raw samples while pressed; the point is taken on release.
    int16_t rx, ry;
    if (Touch::readRaw(rx, ry))
    {
      sumX += rx;
      sumY += ry;
      samples++;
    }
    else if (samples >= kMinSamples)
    {
      rawX[point] = static_cast<int16_t>(sumX / samples);
      rawY[point] = static_cast<int16_t>(sumY / samples);
      sumX = sumY = 0;
      samples = 0;
      if (++point == kPoints)
      {
        failed = !computeAndSave();
        stage = failed ? Stage::Intro : Stage::Done;
        point = 0;
        // The last lift must not land on a Retry or Done button.
        Touch::input.suppressUntilRelease();
      }
    }
    else if (samples > 0 && samples < kMinSamples)
    {
      sumX = sumY = 0; // a brush, not a press
      samples = 0;
    }
  }
}

const Screen2 TouchCalScreen = [] {
  Screen2 s{};
  s.name = "Touch cal";
  s.draw = draw;
  s.update = update;
  s.onEnter = enter;
  s.redrawMs = 0; // the marker and test cross follow the finger
  return s;
}();
