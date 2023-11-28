#pragma once

#include <Arduino.h>

class Battery
{
public:
  Battery(int _pin);

  float getVoltage();
  void update();

private:
  int pin = -1;
  float voltage = -1;
};

Battery::Battery(int _pin)
{
  pin = _pin;
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
