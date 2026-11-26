#include "Buttons.h"

namespace
{
  Controls::Button::Config buttonConfig(int pin)
  {
    Controls::Button::Config c;
    c.pin = pin;
    c.debounceMs = 10;
    c.multiClickMs = 100;
    c.longMs = 400;
    return c;
  }
}

Controls::Button btnUp(buttonConfig(BTN_UP_PIN));
Controls::Button btnDown(buttonConfig(BTN_DOWN_PIN));
Controls::Button btnTrigger(buttonConfig(BTN_TRIGGER_PIN));

void Buttons::setup()
{
  Controls::add(btnUp);
  Controls::add(btnDown);
  Controls::add(btnTrigger);
}

void Buttons::update()
{
  Controls::update();
}

Buttons buttons;
