// TFT_eSPI sprite-backed driver implementation.
#include "TFTDisplayDriver.h"
#include "display_config.h"

TFTDisplayDriver::TFTDisplayDriver() : tft(), sprite(&tft)
{
}

void TFTDisplayDriver::init()
{
  sprite.createSprite(LCD_WIDTH, LCD_HEIGHT);
  sprite.setSwapBytes(1);

  tft.begin();
  tft.setRotation(TFT_ROT);
}

uint16_t TFTDisplayDriver::width() const
{
  return LCD_WIDTH;
}

uint16_t TFTDisplayDriver::height() const
{
  return LCD_HEIGHT;
}

void TFTDisplayDriver::clear(uint16_t color)
{
  sprite.fillSprite(color);
}

void TFTDisplayDriver::push()
{
  sprite.pushSprite(0, 0);
}

void TFTDisplayDriver::setTextSize(uint8_t size)
{
  sprite.setTextSize(size);
}

void TFTDisplayDriver::setTextDatum(uint8_t datum)
{
  sprite.setTextDatum(datum);
}

void TFTDisplayDriver::setTextColor(uint16_t color)
{
  sprite.setTextColor(color);
}

void TFTDisplayDriver::setTextColor(uint16_t fg, uint16_t bg)
{
  sprite.setTextColor(fg, bg);
}

void TFTDisplayDriver::drawString(const String &text, int32_t x, int32_t y)
{
  sprite.drawString(text, x, y);
}

void TFTDisplayDriver::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
{
  sprite.drawLine(x0, y0, x1, y1, color);
}

void TFTDisplayDriver::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
  sprite.drawRect(x, y, w, h, color);
}

void TFTDisplayDriver::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
  sprite.fillRect(x, y, w, h, color);
}

void TFTDisplayDriver::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
{
  sprite.drawRoundRect(x, y, w, h, r, color);
}

void TFTDisplayDriver::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
{
  sprite.fillRoundRect(x, y, w, h, r, color);
}

int16_t TFTDisplayDriver::textWidth(const String &text)
{
  return sprite.textWidth(text);
}

int16_t TFTDisplayDriver::fontHeight()
{
  return sprite.fontHeight();
}

uint16_t TFTDisplayDriver::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return tft.color565(r, g, b);
}
