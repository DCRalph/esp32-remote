// Small centred toast, drawn over whatever the current screen has already drawn.
//
// dcr_display's Popup is a modal: ScreenManager stops calling screen->update()
// while one is open. These notifications are informational only and must not
// pause the screen behind them, so screens draw them at the end of their own
// draw() instead.
#pragma once

#include <Arduino.h>

namespace Notification
{
  void show(const String &message, uint32_t durationMs = 2000);
  void hide();
  bool isActive();

  /** Call last in a screen's draw() so the toast lands on top. */
  void draw();
}
