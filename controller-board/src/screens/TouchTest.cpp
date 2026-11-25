// Draw with a finger and read the raw controller values: checks calibration
// (the crosshair should sit under the finger) and the pressure thresholds.
#include "screens/Screens.h"

#include <vector>

#include "Touch.h"
#include "config.h"

namespace
{
  struct Point
  {
    int16_t x, y;
    bool start; // first point of a stroke
  };

  constexpr size_t kMaxPoints = 2000;
  constexpr int16_t kTop = 24;    // below the top bar
  constexpr int16_t kFooter = 36; // button row

  std::vector<Point> points;
  bool drawing = false;
  int16_t lastX = -1, lastY = -1;

  Button clearButton("Clear", []()
                     { points.clear(); });
  Button backButton("Back", []()
                    { screenManager.back(); });
  FocusGroup group;
  Rect canvas;
  bool built = false;

  void text(const String &s, int16_t x, int16_t y, uint8_t datum, uint16_t color)
  {
    display.setTextRole(TextRole::Small);
    display.setTextColor(color);
    display.setTextDatum(datum);
    display.drawString(s, x, y);
  }

  void enter()
  {
    if (!built)
    {
      built = true;
      points.reserve(kMaxPoints);
      canvas = {0, kTop, static_cast<int16_t>(LCD_WIDTH), static_cast<int16_t>(LCD_HEIGHT - kTop - kFooter)};
      clearButton.bounds = {6, static_cast<int16_t>(LCD_HEIGHT - 32), 90, 28};
      backButton.bounds = {static_cast<int16_t>(LCD_WIDTH - 96), static_cast<int16_t>(LCD_HEIGHT - 32), 90, 28};
      group.add(clearButton);
      group.add(backButton);
    }
    lastX = lastY = -1;
  }

  void draw()
  {
    const Theme &t = display.theme();
    const Point *prev = nullptr;
    for (const Point &p : points)
    {
      if (!p.start && prev)
        display.drawLine(prev->x, prev->y, p.x, p.y, t.accent);
      else
        display.drawPixel(p.x, p.y, t.accent);
      prev = &p;
    }
    if (lastX >= 0)
    {
      display.drawLine(lastX - 10, lastY, lastX + 10, lastY, t.danger);
      display.drawLine(lastX, lastY - 10, lastX, lastY + 10, t.danger);
    }

    // Live controller values, unfiltered, next to the filtered pointer.
    const Touch::Reading r = Touch::read();
    text("raw " + String(r.rawX) + "," + String(r.rawY) + "  z " + String(r.z), 4, kTop + 2,
         Datum::TopLeft, r.touched ? t.success : t.mutedText);
    text(lastX >= 0 ? "px " + String(lastX) + "," + String(lastY) : String("px -"), LCD_WIDTH - 4, kTop + 2,
         Datum::TopRight, t.text);
    const int16_t barW = static_cast<int16_t>(std::min<uint32_t>(r.z, 2000) * (LCD_WIDTH - 8) / 2000);
    display.fillRect(4, kTop + 16, barW, 3, r.z > TouchFilter::kStartPressure ? t.success : t.track);
    text(String(points.size()) + " pts", LCD_WIDTH / 2, LCD_HEIGHT - 18, Datum::MiddleCenter, t.mutedText);

    group.draw();
  }

  void update()
  {
    for (const InputEvent &e : Inputs::events())
    {
      const bool inCanvas = canvas.contains(e.x, e.y);
      if (e.kind == InputKind::PointerDown)
        drawing = inCanvas;
      else if (e.kind == InputKind::PointerUp)
        drawing = false;
      if (e.kind == InputKind::PointerDown || e.kind == InputKind::PointerMove)
      {
        lastX = e.x;
        lastY = e.y;
      }
      if (drawing && inCanvas && points.size() < kMaxPoints &&
          (e.kind == InputKind::PointerDown || e.kind == InputKind::PointerMove))
        points.push_back({e.x, e.y, e.kind == InputKind::PointerDown});
    }
    group.update();
  }
}

const Screen2 TouchTestScreen = [] {
  Screen2 s{};
  s.name = "Touch test";
  s.draw = draw;
  s.update = update;
  s.onEnter = enter;
  s.redrawMs = 0; // the controller readout is live
  return s;
}();
