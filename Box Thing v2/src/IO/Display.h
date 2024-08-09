#pragma once

#include "config.h"
#include <U8g2lib.h>

#include "IO/ScreenManager.h"

class Display
{
private:
  bool refreshScreen = false;
  unsigned long lastRefresh = 0;

public:
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2 = U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);

  Display();

  void init(void);

  void drawCenteredText(uint8_t y, String text);

  void drawTopBar();
  void refresh(void);
  void display(void);
};

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