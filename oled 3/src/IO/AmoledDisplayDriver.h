// LilyGO T-Display AMOLED (RM67162). TFT_eSPI holds the frame in a sprite; the
// panel itself is driven by driver/rm67162 over QSPI.
#pragma once

#include <TFTDisplayDriver.h>

class AmoledDisplayDriver : public TFTDisplayDriver
{
public:
  using TFTDisplayDriver::TFTDisplayDriver;

protected:
  void initPanel() override;
  void pushFrame() override;
  // TFT_eSPI is not wired to this panel, so there is no direct-draw fallback.
  bool allowDirectFallback() const override { return false; }
};
