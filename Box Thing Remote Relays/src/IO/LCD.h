
#pragma once

#include "config.h"

#include <LiquidCrystal_I2C.h>

struct Screen
{
  const char *name;
  void (*draw)();
  void (*update)();
  void (*onEnter)();
  void (*onExit)();
};

class LCD
{
private:
public:
  LiquidCrystal_I2C lcd;

  //  pointer to current screen
  Screen *currentScreen = nullptr;

  LCD();

  void init();
  void loop();

  void setScreen(Screen *screen);
};

extern LCD lcd;