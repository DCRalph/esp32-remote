// Both screens. The built-in LCD is the library's global `display` (with
// `screenManager`) and takes the two buttons; the ILI9341 is `touchDisplay`
// (with `touchScreens`) and takes touch. Render both from loop().
#pragma once

#include <dcr_display.h>

extern Display touchDisplay;
extern ScreenManager touchScreens;

namespace Displays
{
  /** Brings up both panels, their inputs and their home screens. */
  void begin();
  /** One frame on each display, timed under PROFILE_LCD and PROFILE_TOUCH. */
  void render();
}
