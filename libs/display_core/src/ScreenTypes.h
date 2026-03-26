#pragma once

#include <Arduino.h>

#ifndef TFT_BLACK
#define TFT_BLACK 0x0000
#endif
#ifndef TFT_BLUE
#define TFT_BLUE 0x001F
#endif

/** Screen descriptor: function-pointer table + top bar styling (replaces class Screen). */
struct Screen2
{
  const char *name;
  void (*draw)();
  void (*update)();
  void (*onEnter)();
  void (*onExit)();
  uint16_t topBarColor = TFT_BLUE;
  uint16_t topBarTextColor = TFT_BLACK;
};

using ScreenBackgroundStep = void (*)(const Screen2 *screen);
