// The 128x64 mono top bar: screen name, link icon, battery percentage.
#pragma once

#include <Display.h>
#include <ScreenTypes.h>

namespace TopBar
{
  /** Height in pixels, including the separator line. Menus start below this. */
  constexpr uint16_t kHeight = 12;

  /** Install as dcr_display's top bar renderer. */
  void render(Display &display, const Screen2 *screen);

  /**
   * X coordinate for a screen-supplied icon of `width` px, walking leftwards from
   * the link icon. Reset each frame by `render`.
   */
  int nextIconX(int width);
}
