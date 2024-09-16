#pragma once

#include <Arduino.h>

class Battery
{
public:
  Battery(int _pin);

  void init(void);

  float getVoltage();
  void update();

private:
  int pin = -1;
  float voltage = -1;
};

extern Battery battery;
