// Touch calibration across both screens: steps on the LCD, targets on the panel.
#include "screens/Screens.h"

#include "config.h"
#include "Displays.h"
#include "IO/Touch.h"

namespace
{
  constexpr int kInset = 30;
  constexpr int kPoints = 4; // TL, TR, BR, BL
  constexpr int kMinSamples = 5;

  enum class Stage
  {
    Intro,
    Collect,
    Done
  };

  Stage stage = Stage::Intro;
  int point = 0;
  long sumX = 0, sumY = 0;
  int samples = 0;
  int16_t rawX[kPoints], rawY[kPoints];
  int16_t testX = -1, testY = -1;

  void target(int i, int16_t &x, int16_t &y)
  {
    x = (i == 1 || i == 2) ? TOUCH_WIDTH - 1 - kInset : kInset;
    y = (i >= 2) ? TOUCH_HEIGHT - 1 - kInset : kInset;
  }

  // Average each edge's two readings, then extrapolate from the inset targets to the edges.
  void computeAndSave()
  {
    const float left = (rawX[0] + rawX[3]) / 2.0f, right = (rawX[1] + rawX[2]) / 2.0f;
    const float topY = (rawY[0] + rawY[1]) / 2.0f, bottom = (rawY[2] + rawY[3]) / 2.0f;
    const float ppx = (right - left) / ((TOUCH_WIDTH - 1) - 2 * kInset);
    const float ppy = (bottom - topY) / ((TOUCH_HEIGHT - 1) - 2 * kInset);
    Touch::setCalibration({static_cast<int16_t>(lroundf(left - ppx * kInset)),
                           static_cast<int16_t>(lroundf(right + ppx * kInset)),
                           static_cast<int16_t>(lroundf(topY - ppy * kInset)),
                           static_cast<int16_t>(lroundf(bottom + ppy * kInset))});
    Touch::save();
  }

  void lcdEnter()
  {
    stage = Stage::Intro;
    point = samples = 0;
    sumX = sumY = 0;
    testX = testY = -1;
    touchScreens.setScreen(&Demos::TouchCalTouch);
  }

  void lcdExit()
  {
    touchScreens.setScreen(&TouchHomeScreen);
  }

  void lcdDraw()
  {
    const int16_t y = MenuStyle::contentTopY() + 6;
    switch (stage)
    {
    case Stage::Intro:
      drawText(display, "Tap the 4 markers on", 8, y);
      drawText(display, "the touch screen.", 8, y + 24);
      drawText(display, "Double click: start", 8, y + 60, TextRole::Body, Datum::TopLeft, Color::Green);
      drawText(display, "Triple click: cancel", 8, y + 84, TextRole::Body, Datum::TopLeft, Color::DarkGrey);
      break;
    case Stage::Collect:
      drawText(display, "Tap the marker", 8, y);
      drawText(display, String("Point ") + (point + 1) + " of " + kPoints, 8, y + 30, TextRole::Title,
               Datum::TopLeft, Color::Cyan);
      break;
    case Stage::Done:
      drawText(display, "Saved", 8, y, TextRole::Title, Datum::TopLeft, Color::Green);
      drawText(display, "Touch the panel to test.", 8, y + 36);
      drawText(display, "Double click: done", 8, y + 70, TextRole::Body, Datum::TopLeft, Color::DarkGrey);
      break;
    }
  }

  void lcdUpdate()
  {
    for (const InputEvent &e : Inputs::events())
    {
      if (isExitEvent(e))
      {
        screenManager.back();
        return;
      }
      if (e.kind == InputKind::Select && e.value == 1)
      {
        if (stage == Stage::Intro)
          stage = Stage::Collect;
        else if (stage == Stage::Done)
        {
          screenManager.back();
          return;
        }
      }
    }

    // Raw samples while pressed; the point is taken on release.
    int16_t rx, ry;
    if (stage == Stage::Collect)
    {
      if (Touch::readRaw(rx, ry))
      {
        sumX += rx;
        sumY += ry;
        samples++;
      }
      else if (samples >= kMinSamples)
      {
        rawX[point] = sumX / samples;
        rawY[point] = sumY / samples;
        sumX = sumY = 0;
        samples = 0;
        if (++point == kPoints)
        {
          computeAndSave();
          stage = Stage::Done;
        }
      }
    }
    else if (stage == Stage::Done)
      Touch::read(testX, testY);
  }

  void touchDraw()
  {
    Display &d = touchDisplay;
    d.noTopBar();
    if (stage == Stage::Collect)
    {
      int16_t x, y;
      target(point, x, y);
      d.drawSmoothArc(x, y, 12, 9, 0, 0, Color::Yellow, d.theme().background, false);
      d.drawLine(x - 18, y, x + 18, y, Color::Red);
      d.drawLine(x, y - 18, x, y + 18, Color::Red);
      return;
    }
    drawText(d, stage == Stage::Intro ? "Calibration: see LCD" : "Touch to test", d.width() / 2, d.height() / 2,
             TextRole::Body, Datum::MiddleCenter);
    if (stage == Stage::Done && testX >= 0)
      d.fillRoundRect(testX - 4, testY - 4, 8, 8, 4, Color::Green);
  }
}

const Screen2 TouchCalScreen = {
    .name = "Touch cal",
    .draw = lcdDraw,
    .update = lcdUpdate,
    .onEnter = lcdEnter,
    .onExit = lcdExit,
};

const Screen2 Demos::TouchCalTouch = {
    .name = "Touch cal",
    .draw = touchDraw,
    .update = nullptr,
    .onEnter = nullptr,
    .onExit = nullptr,
};
