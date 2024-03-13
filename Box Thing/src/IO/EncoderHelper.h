#pragma once

#include "config.h"
#include <RotaryEncoder.h>

class EncoderHelper
{
private:
public:
  RotaryEncoder encoder = RotaryEncoder(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_PIN_BUTTON);

  void init(void);
};