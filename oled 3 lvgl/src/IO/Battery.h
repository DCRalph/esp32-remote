#pragma once

#include "config.h"

class Battery
{
public:
  Battery(int _pin);

  void init(void);

  void update();

  float getVoltage();
  float getPercentageF();
  int getPercentageI();
  int getPercentage10();

private:
  int pin = -1;
  float voltage = -1;
};

extern Battery battery;
