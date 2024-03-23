#include "EncoderHelper.h"

void EncoderHelper::init(void)
{
  Serial.println("\t[Encoder] Initializing...");
  encoder.begin();

  Serial.println("\t[Encoder] Initialized");
}
