#include "Battery.h"

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

void Battery::update()
{
  if (pin != -1)
  {
    voltage = ((analogRead(pin) * 2 * 3.3 * 1000) / 4096) / 1000;
  }
}

float Battery::getVoltage()
{
  return voltage;
}

float Battery::getPercentageF()
{



  return (voltage - BATT_MIN) * (100 - 0) / (BATT_MAX - BATT_MIN) + 0;
}

int Battery::getPercentageI()
{
  return (int)getPercentageF();
}

int Battery::getPercentage10()
{
  int percent = getPercentageI();

  percent = round(percent / 10);

  return percent * 10;
}

Battery battery(4);