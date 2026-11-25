#include "IO/AmoledDisplayDriver.h"

#include <esp_heap_caps.h>

#include "driver/rm67162.h"

namespace
{
  constexpr size_t kScratchPx = 8192; // 16 KB
}

AmoledDisplayDriver::~AmoledDisplayDriver()
{
  heap_caps_free(scratch);
}

void AmoledDisplayDriver::initPanel()
{
  rm67162_init();
  lcd_setRotation(rotation);
  lcd_brightness(255);
}

void AmoledDisplayDriver::pushFrame()
{
  lcd_PushColors(0, 0, frame.width(), frame.height(), static_cast<uint16_t *>(frame.getPointer()));
}

void AmoledDisplayDriver::pushRegion(int16_t x, int16_t y, int16_t rw, int16_t rh)
{
  auto *pixels = static_cast<uint16_t *>(frame.getPointer());
  const int16_t stride = frame.width();
  if (x == 0 && rw == stride)
  {
    lcd_PushColors(0, y, rw, rh, pixels + static_cast<size_t>(y) * stride);
    return;
  }

  if (!scratch)
    scratch = static_cast<uint16_t *>(heap_caps_malloc(kScratchPx * sizeof(uint16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
  // Even row counts: the RM67162 wants even-aligned address windows.
  const int16_t chunkRows = scratch ? max<int16_t>(2, (kScratchPx / rw) & ~1) : 0;
  if (!scratch || static_cast<size_t>(chunkRows) * rw > kScratchPx)
  {
    lcd_PushColors(0, y, stride, rh, pixels + static_cast<size_t>(y) * stride);
    return;
  }

  for (int16_t row = 0; row < rh; row += chunkRows)
  {
    const int16_t n = min<int16_t>(chunkRows, rh - row);
    for (int16_t r = 0; r < n; r++)
      memcpy(scratch + static_cast<size_t>(r) * rw, pixels + static_cast<size_t>(y + row + r) * stride + x,
             rw * sizeof(uint16_t));
    lcd_PushColors(x, y + row, rw, n, scratch);
  }
}
