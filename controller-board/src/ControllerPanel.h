// The board's TFT_eSPI driver, with the two handles this project needs that the
// public accessors (tft(), sprite()) would cost: those switch off the fast fills.
#pragma once

#include <TFTDisplayDriver.h>

class ControllerPanel : public TFTDisplayDriver
{
public:
  using TFTDisplayDriver::TFTDisplayDriver;

  /** The panel itself: touch reads, and calibration drawing (call resync() after). */
  TFT_eSPI &panelDevice() { return panel; }

  /** The frame as pushed, byte-swapped RGB565 (TFT_eSprite's layout); null without a sprite. */
  const uint16_t *frameBuffer() { return spriteReady ? static_cast<const uint16_t *>(frame.getPointer()) : nullptr; }
};
