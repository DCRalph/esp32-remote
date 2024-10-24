#include "headLights.h"

HeadLightsDecoder::HeadLightsDecoder()
{
  decoderId = 0x164;
}

void HeadLightsDecoder::process(const canFrame &frame)
{
  // Decode RPM data from the frame
  uint8_t state = frame.data[0];

  result = static_cast<HeadLightsState>(state);
}