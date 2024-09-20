#pragma once

#include "config.h"
#include <TFT_eSPI.h>
#include "driver/rm67162.h"
#include "ScreenManager.h"
#include "Menu.h"

#include "battery.h"

class Screen;

class Display
{
private:
  bool showMenuBar = true;

public:
  TFT_eSPI tft = TFT_eSPI();
  TFT_eSprite sprite = TFT_eSprite(&tft);

  void init(void);

  void drawTopBar(void);
  void clearScreen(u16_t color = TFT_BLACK);

  void wrapTextInBounds(String text, int x, int y, int width, int height, int textSize, int gap, u16_t color, int textAlignment = TC_DATUM);

  void noTopBar(void);

  void push(void);

  void display(void);
};

//****************************************************

class Screen
{
public:
  String name;

  uint16_t topBarColor = TFT_MAGENTA;
  uint16_t topBarTextColor = TFT_BLACK;

  /**
   * @brief Constructs a new Screen object with the specified name.
   *
   * @param _name The name of the screen.
   */
  Screen(String _name);

  /**
   * @brief Draws the screen.
   */
  virtual void draw();

  /**
   * @brief Updates the screen.
   */
  virtual void update();

  /**
   * @brief Called when the screen is entered.
   */
  virtual void onEnter();

  /**
   * @brief Called when the screen is exited.
   */
  virtual void onExit();
};

extern Display display;