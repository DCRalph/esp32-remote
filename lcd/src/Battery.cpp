#include "battery.h"

Battery::Battery(int _pin)
{
  pin = _pin;
}

void Battery::init(void)
{
  if (pin != -1)
  {
    pinMode(pin, INPUT);
  }
}

float Battery::getVoltage()
{
  return voltage;
}

void Battery::update()
{
  if (pin != -1)
  {
    voltage = ((analogRead(pin) * 2 * 3.3 * 1000) / 4096) / 1000;
  }
}

Battery battery(4);