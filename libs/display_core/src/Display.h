// Display abstraction and facade.
#pragma once

#include <Arduino.h>

// Display default colors/datum (match TFT_eSPI when used with TFT driver)
#ifndef TFT_BLACK
#define TFT_BLACK 0x0000
#endif
#ifndef TFT_BLUE
#define TFT_BLUE 0x001F
#endif
#ifndef TC_DATUM
#define TC_DATUM 1
#endif
#ifndef TL_DATUM
#define TL_DATUM 0
#endif
#ifndef TR_DATUM
#define TR_DATUM 2
#endif
#ifndef ML_DATUM
#define ML_DATUM 3
#endif
#ifndef MR_DATUM
#define MR_DATUM 5
#endif
#ifndef TFT_WHITE
#define TFT_WHITE 0xFFFF
#endif
#ifndef TFT_RED
#define TFT_RED 0xF800
#endif

class ScreenManager;
struct DisplayConfig;

using DisplayInitFn = void (*)(void *context, const DisplayConfig &config);
using DisplayClearFn = void (*)(void *context, uint16_t color);
using DisplayPushFn = void (*)(void *context);
using DisplaySetTextSizeFn = void (*)(void *context, uint8_t size);
using DisplaySetTextDatumFn = void (*)(void *context, uint8_t datum);
using DisplaySetTextColorFn = void (*)(void *context, uint16_t color);
using DisplaySetTextColorBgFn = void (*)(void *context, uint16_t fg, uint16_t bg);
using DisplayDrawStringFn = void (*)(void *context, const String &text, int32_t x, int32_t y);
using DisplayDrawLineFn = void (*)(void *context, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color);
using DisplayDrawRectFn = void (*)(void *context, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
using DisplayFillRectFn = void (*)(void *context, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
using DisplayDrawRoundRectFn = void (*)(void *context, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);
using DisplayFillRoundRectFn = void (*)(void *context, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);
using DisplayDrawSmoothArcFn = void (*)(void *context, int32_t x, int32_t y, int32_t r, int32_t ir, int32_t startAngle, int32_t endAngle, uint16_t fgColor, uint16_t bgColor, bool roundEnds);
using DisplayTextWidthFn = int16_t (*)(void *context, const String &text);
using DisplayFontHeightFn = int16_t (*)(void *context);
using DisplayColor565Fn = uint16_t (*)(void *context, uint8_t r, uint8_t g, uint8_t b);

struct DisplayDriverOps
{
  DisplayInitFn init = nullptr;
  DisplayClearFn clear = nullptr;
  DisplayClearFn fillScreen = nullptr;
  DisplayPushFn push = nullptr;

  DisplaySetTextSizeFn setTextSize = nullptr;
  DisplaySetTextDatumFn setTextDatum = nullptr;
  DisplaySetTextColorFn setTextColor = nullptr;
  DisplaySetTextColorBgFn setTextColorBg = nullptr;

  DisplayDrawStringFn drawString = nullptr;
  DisplayDrawLineFn drawLine = nullptr;
  DisplayDrawRectFn drawRect = nullptr;
  DisplayFillRectFn fillRect = nullptr;
  DisplayDrawRoundRectFn drawRoundRect = nullptr;
  DisplayFillRoundRectFn fillRoundRect = nullptr;
  DisplayDrawSmoothArcFn drawSmoothArc = nullptr;

  DisplayTextWidthFn textWidth = nullptr;
  DisplayFontHeightFn fontHeight = nullptr;
  DisplayColor565Fn color565 = nullptr;
};

struct DisplayConfig
{
  uint16_t width = 0;
  uint16_t height = 0;
  uint8_t rotation = 0;
  uint8_t colorDepth = 16;
  void *context = nullptr;
  DisplayDriverOps ops;
};

class Screen;

class Display
{
public:
  Display();

  void begin(const DisplayConfig &config, ScreenManager *screenManager = nullptr);
  void setScreenManager(ScreenManager *screenManager);

  bool isReady() const;
  uint16_t width() const;
  uint16_t height() const;
  uint8_t rotation() const;
  uint8_t colorDepth() const;

  void clearScreen(uint16_t color = TFT_BLACK);
  void fillScreen(uint16_t color = TFT_BLACK);
  void push();

  void setTextSize(uint8_t size);
  void setTextDatum(uint8_t datum);
  void setTextColor(uint16_t color);
  void setTextColor(uint16_t fg, uint16_t bg);

  void drawString(const String &text, int32_t x, int32_t y);
  void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color);
  void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
  void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
  void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);
  void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);
  void drawSmoothArc(int32_t x, int32_t y, int32_t r, int32_t ir, int32_t startAngle, int32_t endAngle, uint16_t fgColor, uint16_t bgColor, bool roundEnds);

  int16_t textWidth(const String &text);
  int16_t fontHeight();

  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

  void wrapTextInBounds(String text, int x, int y, int width, int height, int textSize, int gap, uint16_t color, int textAlignment = TC_DATUM);

  void noTopBar();
  void drawTopBar();

  void render();
  void render(ScreenManager &manager);

private:
  DisplayConfig config;
  ScreenManager *screenManager;
  bool showMenuBar;
};

class Screen
{
public:
  String name;

  uint16_t topBarColor = TFT_BLUE;
  uint16_t topBarTextColor = TFT_BLACK;

  Screen(String _name);

  virtual void draw();
  virtual void update();
  virtual void onEnter();
  virtual void onExit();
};

extern Display display;

// Provided by application
int getBatteryPercentage();
