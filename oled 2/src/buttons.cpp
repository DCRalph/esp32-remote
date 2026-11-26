#include "buttons.h"

namespace
{
  // ClickButton's defaults, which these buttons have always run on.
  Controls::Button::Config buttonConfig(int pin)
  {
    Controls::Button::Config c;
    c.pin = pin;
    c.debounceMs = 20;
    c.multiClickMs = 250;
    c.longMs = 1000;
    return c;
  }
}

Controls::Button btn21(buttonConfig(21));
Controls::Button btn0(buttonConfig(0));
