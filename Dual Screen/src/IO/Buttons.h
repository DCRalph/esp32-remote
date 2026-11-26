#pragma once

#include "config.h"
#include <dcr_input.h>

extern Controls::Button btnUp;
extern Controls::Button btnDown;

class Buttons
{
public:
  void setup();
  void update();
};

extern Buttons buttons;
