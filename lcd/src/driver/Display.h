#pragma once

#include "Arduino.h"
#include <TFT_eSPI.h>
#include "driver/pins_config.h"
#include "driver/ScreenManager.h"
#include "driver/ScreenUtils.h"
#include "WiFi.h"

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

  void noTopBar(void);

  void push(void);

  void display(void);
};

//****************************************************

class Screen
{
public:
  String name;

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