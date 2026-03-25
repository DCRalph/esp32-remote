#include "AmoledDisplayDriver.h"

namespace
{
  static AmoledDisplayDriverContext *ctx(void *context)
  {
    return static_cast<AmoledDisplayDriverContext *>(context);
  }

  static void init(void *context, const DisplayConfig &config)
  {
    AmoledDisplayDriverContext *c = ctx(context);
    c->sprite.createSprite(config.width, config.height);
    c->sprite.setSwapBytes(1);

    rm67162_init();
    lcd_setRotation(config.rotation);
    lcd_brightness(255);
  }

  static void clear(void *context, uint16_t color)
  {
    AmoledDisplayDriverContext *c = ctx(context);
    c->sprite.fillRect(0, 0, c->sprite.width(), c->sprite.height(), color);
  }

  static void fillScreen(void *context, uint16_t color)
  {
    ctx(context)->sprite.fillScreen(color);
  }

  static void push(void *context)
  {
    AmoledDisplayDriverContext *c = ctx(context);
    lcd_PushColors(0, 0, c->sprite.width(), c->sprite.height(), (uint16_t *)c->sprite.getPointer());
  }

  static void setTextSize(void *context, uint8_t size)
  {
    ctx(context)->sprite.setTextSize(size);
  }

  static void setTextDatum(void *context, uint8_t datum)
  {
    ctx(context)->sprite.setTextDatum(datum);
  }

  static void setTextColor(void *context, uint16_t color)
  {
    ctx(context)->sprite.setTextColor(color);
  }

  static void setTextColorBg(void *context, uint16_t fg, uint16_t bg)
  {
    ctx(context)->sprite.setTextColor(fg, bg);
  }

  static void drawString(void *context, const String &text, int32_t x, int32_t y)
  {
    ctx(context)->sprite.drawString(text, x, y);
  }

  static void drawLine(void *context, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
  {
    ctx(context)->sprite.drawLine(x0, y0, x1, y1, color);
  }

  static void drawRect(void *context, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
  {
    ctx(context)->sprite.drawRect(x, y, w, h, color);
  }

  static void fillRect(void *context, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
  {
    ctx(context)->sprite.fillRect(x, y, w, h, color);
  }

  static void drawRoundRect(void *context, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
  {
    ctx(context)->sprite.drawRoundRect(x, y, w, h, r, color);
  }

  static void fillRoundRect(void *context, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
  {
    ctx(context)->sprite.fillRoundRect(x, y, w, h, r, color);
  }

  static void drawSmoothArc(void *context, int32_t x, int32_t y, int32_t r, int32_t ir, int32_t startAngle, int32_t endAngle, uint16_t fgColor, uint16_t bgColor, bool roundEnds)
  {
    ctx(context)->sprite.drawSmoothArc(x, y, r, ir, startAngle, endAngle, fgColor, bgColor, roundEnds);
  }

  static int16_t textWidth(void *context, const String &text)
  {
    return ctx(context)->sprite.textWidth(text);
  }

  static int16_t fontHeight(void *context)
  {
    return ctx(context)->sprite.fontHeight();
  }

  static uint16_t color565(void *context, uint8_t r, uint8_t g, uint8_t b)
  {
    return ctx(context)->tft.color565(r, g, b);
  }
} // namespace

DisplayDriverOps AmoledDisplayDriver::makeOps()
{
  DisplayDriverOps ops;
  ops.init = init;
  ops.clear = clear;
  ops.fillScreen = fillScreen;
  ops.push = push;
  ops.setTextSize = setTextSize;
  ops.setTextDatum = setTextDatum;
  ops.setTextColor = setTextColor;
  ops.setTextColorBg = setTextColorBg;
  ops.drawString = drawString;
  ops.drawLine = drawLine;
  ops.drawRect = drawRect;
  ops.fillRect = fillRect;
  ops.drawRoundRect = drawRoundRect;
  ops.fillRoundRect = fillRoundRect;
  ops.drawSmoothArc = drawSmoothArc;
  ops.textWidth = textWidth;
  ops.fontHeight = fontHeight;
  ops.color565 = color565;
  return ops;
}

DisplayConfig AmoledDisplayDriver::makeConfig(AmoledDisplayDriverContext *context, uint16_t width, uint16_t height, uint8_t rotation, uint8_t colorDepth)
{
  DisplayConfig config;
  config.width = width;
  config.height = height;
  config.rotation = rotation;
  config.colorDepth = colorDepth;
  config.context = context;
  config.ops = makeOps();
  return config;
}
