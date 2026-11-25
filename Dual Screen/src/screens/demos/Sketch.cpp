// Draw on the touch panel; the LCD mirrors it scaled down and picks the pen.
// LCD: up/down change pen, double click clears, triple click exits.
#include "screens/Screens.h"

#include "config.h"
#include "Displays.h"

namespace
{
  struct Point
  {
    int16_t x, y;
    uint8_t pen;
    bool start; // first point of a stroke
  };

  constexpr size_t kMaxPoints = 3000;
  constexpr uint16_t kPens[] = {Color::White, Color::Red, Color::Green, Color::Cyan, Color::Yellow, Color::Magenta};
  constexpr const char *kPenNames[] = {"White", "Red", "Green", "Cyan", "Yellow", "Magenta"};
  constexpr uint8_t kPenCount = sizeof(kPens) / sizeof(kPens[0]);

  std::vector<Point> points;
  uint8_t pen = 0;
  bool drawing = false;

  Button clearButton("Clear", []()
                     { points.clear(); });
  Button backButton("Back", []()
                    { screenManager.back(); });
  FocusGroup group;
  Rect canvas;
  bool built = false;

  void build()
  {
    built = true;
    points.reserve(kMaxPoints);
    canvas = {0, 22, TOUCH_WIDTH, static_cast<int16_t>(TOUCH_HEIGHT - 22 - 36)};
    clearButton.bounds = {6, static_cast<int16_t>(TOUCH_HEIGHT - 32), 90, 28};
    backButton.bounds = {static_cast<int16_t>(TOUCH_WIDTH - 96), static_cast<int16_t>(TOUCH_HEIGHT - 32), 90, 28};
    group.add(clearButton);
    group.add(backButton);
  }

  // Draw every stroke into `d`, mapped from panel space into `area`.
  void drawStrokes(Display &d, const Rect &area)
  {
    const Point *prev = nullptr;
    for (const Point &p : points)
    {
      const int16_t x = area.x + (p.x - canvas.x) * area.w / canvas.w;
      const int16_t y = area.y + (p.y - canvas.y) * area.h / canvas.h;
      if (!p.start && prev)
      {
        const int16_t px = area.x + (prev->x - canvas.x) * area.w / canvas.w;
        const int16_t py = area.y + (prev->y - canvas.y) * area.h / canvas.h;
        d.drawLine(px, py, x, y, kPens[p.pen]);
      }
      else
        d.drawPixel(x, y, kPens[p.pen]);
      prev = &p;
    }
  }

  void lcdEnter()
  {
    if (!built)
      build();
    touchScreens.setScreen(&Demos::SketchTouch);
  }

  void lcdExit()
  {
    touchScreens.setScreen(&TouchHomeScreen);
  }

  void lcdDraw()
  {
    const int16_t top = MenuStyle::contentTopY() + 2;
    const int16_t h = display.height() - top - 2;
    const Rect preview{2, top, static_cast<int16_t>(h * canvas.w / canvas.h), h};
    display.drawRect(preview.x - 1, preview.y - 1, preview.w + 2, preview.h + 2, Color::DarkGrey);
    drawStrokes(display, preview);

    const int16_t tx = preview.x + preview.w + 8;
    drawText(display, "Pen", tx, top, TextRole::Small, Datum::TopLeft, Color::DarkGrey);
    display.fillRect(tx, top + 14, 20, 20, kPens[pen]);
    drawText(display, kPenNames[pen], tx + 26, top + 16, TextRole::Small);
    drawText(display, String(points.size()) + " pts", tx, top + 44, TextRole::Small);
    drawText(display, "Up/down: pen", tx, top + 72, TextRole::Small, Datum::TopLeft, Color::DarkGrey);
    drawText(display, "2 clicks: clear", tx, top + 86, TextRole::Small, Datum::TopLeft, Color::DarkGrey);
    drawText(display, "3 clicks: exit", tx, top + 100, TextRole::Small, Datum::TopLeft, Color::DarkGrey);
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
      if (e.kind == InputKind::Up)
        pen = (pen + kPenCount - 1) % kPenCount;
      else if (e.kind == InputKind::Down)
        pen = (pen + 1) % kPenCount;
      else if (e.kind == InputKind::Select && e.value == 1)
        points.clear();
    }
  }

  void touchDraw()
  {
    drawStrokes(touchDisplay, canvas);
    group.draw();
  }

  void touchUpdate()
  {
    for (const InputEvent &e : Inputs::events())
    {
      const bool inCanvas = canvas.contains(e.x, e.y);
      if (e.kind == InputKind::PointerDown)
        drawing = inCanvas;
      else if (e.kind == InputKind::PointerUp)
        drawing = false;
      if (drawing && inCanvas && points.size() < kMaxPoints &&
          (e.kind == InputKind::PointerDown || e.kind == InputKind::PointerMove))
        points.push_back({e.x, e.y, pen, e.kind == InputKind::PointerDown});
    }
    group.update();
  }
}

const Screen2 Demos::SketchLcd = {
    .name = "Sketch pad",
    .draw = lcdDraw,
    .update = lcdUpdate,
    .onEnter = lcdEnter,
    .onExit = lcdExit,
};

const Screen2 Demos::SketchTouch = {
    .name = "Draw here",
    .draw = touchDraw,
    .update = touchUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
