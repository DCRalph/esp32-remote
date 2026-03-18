// Display abstraction and facade.
#pragma once

#include <Arduino.h>
#include "display_config.h"

// Display default colors/datum (match TFT_eSPI when used with TFT driver)
#ifndef TFT_BLACK
#define TFT_BLACK 0x0000
#endif
#ifndef TFT_BLUE
#define TFT_BLUE  0x001F
#endif
#ifndef TC_DATUM
#define TC_DATUM  1
#endif
#ifndef TL_DATUM
#define TL_DATUM  0
#endif
#ifndef TR_DATUM
#define TR_DATUM  2
#endif
#ifndef ML_DATUM
#define ML_DATUM  3
#endif
#ifndef MR_DATUM
#define MR_DATUM  5
#endif
#ifndef TFT_WHITE
#define TFT_WHITE 0xFFFF
#endif
#ifndef TFT_RED
#define TFT_RED   0xF800
#endif

class ScreenManager;

class IDisplayDriver
{
public:
  virtual ~IDisplayDriver() = default;

  virtual void init() = 0;

  virtual uint16_t width() const = 0;
  virtual uint16_t height() const = 0;

  virtual void clear(uint16_t color) = 0;
  virtual void push() = 0;

  virtual void setTextSize(uint8_t size) = 0;
  virtual void setTextDatum(uint8_t datum) = 0;
  virtual void setTextColor(uint16_t color) = 0;
  virtual void setTextColor(uint16_t fg, uint16_t bg) = 0;

  virtual void drawString(const String &text, int32_t x, int32_t y) = 0;
  virtual void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) = 0;
  virtual void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) = 0;
  virtual void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) = 0;
  virtual void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color) = 0;
  virtual void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color) = 0;

  virtual int16_t textWidth(const String &text) = 0;
  virtual int16_t fontHeight() = 0;

  virtual uint16_t color565(uint8_t r, uint8_t g, uint8_t b) = 0;
};

class Screen;

class Display
{
public:
  Display();

  void begin(IDisplayDriver *driver, ScreenManager *screenManager = nullptr);
  void setScreenManager(ScreenManager *screenManager);

  bool isReady() const;

  void clearScreen(uint16_t color = TFT_BLACK);
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

  int16_t textWidth(const String &text);
  int16_t fontHeight();

  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

  void wrapTextInBounds(String text, int x, int y, int width, int height, int textSize, int gap, uint16_t color, int textAlignment = TC_DATUM);

  void noTopBar();
  void drawTopBar();

  void render();
  void render(ScreenManager &manager);

private:
  IDisplayDriver *driver;
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
