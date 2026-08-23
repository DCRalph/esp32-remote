// U8g2-backed driver for dcr_display's Display facade (SH1106 128x64, 1-bit).
#pragma once

#include <U8g2lib.h>

#include <Display.h>

/**
 * Text "sizes" understood by this driver. The facade only speaks
 * `setTextSize(uint8_t)`, so the sizes double as a font table. Sizes 1-3 are the
 * ones dcr_display's MenuStyle asks for; the rest exist for app code that wants a
 * specific face. Anything out of range falls back to Small.
 */
enum U8g2TextSize : uint8_t
{
  U8G2_TEXT_SMALL = 1,   // u8g2_font_6x10_tf          menu, MenuSize::Small
  U8G2_TEXT_MEDIUM = 2,  // u8g2_font_doomalpha04_tr   menu, MenuSize::Medium
  U8G2_TEXT_LARGE = 3,   // u8g2_font_profont22_tf     menu, MenuSize::Large
  U8G2_TEXT_BAR = 4,     // u8g2_font_koleeko_tf       top bar
  U8G2_TEXT_MONO12 = 5,  // u8g2_font_profont12_tf
  U8G2_TEXT_MONO10 = 6,  // u8g2_font_profont10_tf
  U8G2_TEXT_TITLE = 7,   // u8g2_font_logisoso16_tf
  U8G2_TEXT_TITLE_XL = 8 // u8g2_font_logisoso24_tr
};

struct U8g2DisplayDriverContext
{
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2{U8G2_R0, U8X8_PIN_NONE};

  // Text state the facade tracks separately from u8g2.
  uint8_t datum = TL_DATUM;
  bool textOn = true; // false when text is drawn in the background colour
};

namespace U8g2DisplayDriver
{
  DisplayDriverOps makeOps();
  DisplayConfig makeConfig(U8g2DisplayDriverContext *context, uint16_t width, uint16_t height, uint8_t rotation = 0);

  /** Select a font directly, for glyph work the facade cannot express. */
  void setFont(U8g2DisplayDriverContext *context, const uint8_t *font);
}
