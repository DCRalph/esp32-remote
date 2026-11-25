// The SH1106 128x64 OLED, on dcr_display's U8g2 driver with this project's fonts.
#pragma once

#include <U8g2lib.h>
#include <U8g2DisplayDriver.h>

// App code picks a face with `display.setTextSize(U8G2_TEXT_*)`.
enum U8g2TextSize : uint8_t
{
  U8G2_TEXT_SMALL = 1,   // u8g2_font_6x10_tf
  U8G2_TEXT_MEDIUM = 2,  // u8g2_font_doomalpha04_tr
  U8G2_TEXT_LARGE = 3,   // u8g2_font_profont22_tf
  U8G2_TEXT_BAR = 4,     // u8g2_font_koleeko_tf, top bar
  U8G2_TEXT_MONO12 = 5,  // u8g2_font_profont12_tf
  U8G2_TEXT_MONO10 = 6,  // u8g2_font_profont10_tf
  U8G2_TEXT_TITLE = 7,   // u8g2_font_logisoso16_tf
  U8G2_TEXT_TITLE_XL = 8 // u8g2_font_logisoso24_tr
};

class BoxThingDisplay : public U8g2DisplayDriver
{
public:
  BoxThingDisplay();
  void setTextScale(uint8_t size) override;

private:
  U8G2_SH1106_128X64_NONAME_F_HW_I2C oled{U8G2_R0, U8X8_PIN_NONE};
};

extern BoxThingDisplay displayDriver;
