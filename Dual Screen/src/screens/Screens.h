// Every screen, by display. A demo is a pair: its LCD half opens the touch half
// on enter and sends the touch display home on exit, so it can be started from
// either screen and always ends in the same place.
#pragma once

#include <dcr_display.h>

// LCD
extern const Screen2 HomeScreen;
extern const Screen2 SettingsScreen;
extern const Screen2 TouchCalScreen;

// Touch panel
extern const Screen2 TouchHomeScreen;

namespace Demos
{
  extern const Screen2 ControlsLcd, ControlsTouch;
  extern const Screen2 SketchLcd, SketchTouch;
  extern const Screen2 CrossoverLcd, CrossoverTouch;
  extern const Screen2 TypingLcd, TypingTouch;
  extern const Screen2 DashboardLcd, DashboardTouch;
  extern const Screen2 TouchCalTouch;
}

/** For screens that set a field designated initialisers would have to skip to. */
inline Screen2 makeScreen(const char *name, void (*draw)(), void (*update)(), void (*onEnter)() = nullptr,
                          void (*onExit)() = nullptr, int32_t redrawMs = -1)
{
  Screen2 s{};
  s.name = name;
  s.draw = draw;
  s.update = update;
  s.onEnter = onEnter;
  s.onExit = onExit;
  s.redrawMs = redrawMs;
  return s;
}

/** Text helper: one line in the active display's text colour. */
void drawText(Display &d, const String &text, int16_t x, int16_t y, TextRole role = TextRole::Body,
              uint8_t datum = Datum::TopLeft, int32_t color = kThemeColor);

/** True for the exits every input setup can produce: Back, Left or a triple click. */
bool isExitEvent(const InputEvent &e);
