#pragma once

#include "Arduino.h"
#include <TFT_eSPI.h>
#include "driver/rm67162.h"
#include "driver/pins_config.h"
#include "driver/ScreenManager.h"
#include "driver/ScreenUtils.h"

#include "battery.h"

class Screen;

class Display
{
public:
  TFT_eSPI tft = TFT_eSPI();
  TFT_eSprite sprite = TFT_eSprite(&tft);

  void init(void);

  void drawTopBar(void);
  void clearScreen(u16_t color = TFT_BLACK);

  void push(void);

  void display(void);
};

//****************************************************

class Screen
{
public:
  String name;
  String id;

  Screen(String _name, String _id);

  virtual void draw();
  virtual void update();
};

extern Display display;