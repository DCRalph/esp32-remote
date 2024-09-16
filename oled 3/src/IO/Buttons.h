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

  long debounceTime = 10;
  long multiclickTime = 80;
  long longClickTime = 400;

public:
  void setup();
  void update();
};

extern Buttons buttons;