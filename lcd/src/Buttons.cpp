#include "Buttons.h"

namespace
{
  Controls::Button::Config buttonConfig(int pin)
  {
    Controls::Button::Config c;
    c.pin = pin;
    c.debounceMs = 20;
    c.multiClickMs = 100;
    c.longMs = 500;
    return c;
  }
}

Controls::Button btnUp(buttonConfig(14));
Controls::Button btnDown(buttonConfig(0));

void Buttons::setup()
{
  Controls::add(btnUp);
  Controls::add(btnDown);
}

void Buttons::update()
{
  Controls::update();
}

Buttons buttons;
