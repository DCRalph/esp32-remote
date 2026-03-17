// TFT_eSPI sprite-backed display driver.
#pragma once

#include <TFT_eSPI.h>

#include "Display.h"

class TFTDisplayDriver : public IDisplayDriver
{
public:
  TFTDisplayDriver();

  void init() override;

  uint16_t width() const override;
  uint16_t height() const override;

  void clear(uint16_t color) override;
  void push() override;

  void setTextSize(uint8_t size) override;
  void setTextDatum(uint8_t datum) override;
  void setTextColor(uint16_t color) override;
  void setTextColor(uint16_t fg, uint16_t bg) override;

  void drawString(const String &text, int32_t x, int32_t y) override;
  void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) override;
  void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) override;
  void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) override;
  void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color) override;
  void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color) override;

  int16_t textWidth(const String &text) override;
  int16_t fontHeight() override;

  uint16_t color565(uint8_t r, uint8_t g, uint8_t b) override;

private:
  TFT_eSPI tft;
  TFT_eSprite sprite;
};
