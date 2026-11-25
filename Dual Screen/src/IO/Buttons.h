#pragma once

#include "config.h"
#include <ClickButton.h>

extern ClickButton ClickButtonUP;
extern ClickButton ClickButtonDOWN;
class Buttons
{
  const int sleepCountdownTime = 3000;
  bool sleepCountdown = false;
  unsigned long sleepCountdownMillis = 0;

  long debounceTime = 20;
  long multiclickTime = 100;
  long longClickTime = 500;

public:
  void setup();
  void update();
};

extern Buttons buttons;