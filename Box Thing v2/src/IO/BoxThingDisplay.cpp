#include "IO/BoxThingDisplay.h"

BoxThingDisplay displayDriver;

// The base binds a reference to `oled` before it is constructed; it is first used in begin().
BoxThingDisplay::BoxThingDisplay() : U8g2DisplayDriver(oled)
{
  fonts.small = u8g2_font_6x10_tf;
  fonts.body = u8g2_font_doomalpha04_tr;
  fonts.title = u8g2_font_profont22_tf;
  fonts.xlarge = u8g2_font_logisoso24_tr;
}

void BoxThingDisplay::setTextScale(uint8_t size)
{
  const uint8_t *font;
  switch (size)
  {
  case U8G2_TEXT_MEDIUM:
    font = u8g2_font_doomalpha04_tr;
    break;
  case U8G2_TEXT_LARGE:
    font = u8g2_font_profont22_tf;
    break;
  case U8G2_TEXT_BAR:
    font = u8g2_font_koleeko_tf;
    break;
  case U8G2_TEXT_MONO12:
    font = u8g2_font_profont12_tf;
    break;
  case U8G2_TEXT_MONO10:
    font = u8g2_font_profont10_tf;
    break;
  case U8G2_TEXT_TITLE:
    font = u8g2_font_logisoso16_tf;
    break;
  case U8G2_TEXT_TITLE_XL:
    font = u8g2_font_logisoso24_tr;
    break;
  default:
    font = u8g2_font_6x10_tf;
    break;
  }
  panel().setFont(font);
  panel().setFontPosTop();
}
