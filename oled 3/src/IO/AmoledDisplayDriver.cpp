#include "IO/AmoledDisplayDriver.h"

#include "driver/rm67162.h"

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
