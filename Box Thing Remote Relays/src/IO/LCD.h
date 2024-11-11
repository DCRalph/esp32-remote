
/**
 * @file LCD.h
 * @brief This file contains the declaration of the Screen struct and the LCD class for managing an LCD screen using the LiquidCrystal_I2C library.
 */

#pragma once

#include "config.h"

#include <LiquidCrystal_I2C.h>

#define LCD_CROSS 0
#define LCD_CHECK 1

static  uint8_t LCD_cross[8] = {
    0b00000,
    0b11011,
    0b01110,
    0b00100,
    0b01110,
    0b11011,
    0b00000,
    0b00000};
static  uint8_t LCD_check[8] = {
    0b00000,
    0b00011,
    0b10110,
    0b11100,
    0b01000,
    0b00000,
    0b00000,
    0b00000};

/**
 * @struct Screen
 * @brief Represents a screen with methods for drawing, updating, entering, and exiting.
 *
 * The Screen struct encapsulates the properties and methods required to manage a screen.
 * It includes the screen's name and function pointers to methods for drawing, updating,
 * entering, and exiting the screen.
 */
struct Screen
{

  /**
   * @brief Constructs a Screen object.
   *
   * @param name The name of the screen.
   * @param draw A function pointer to the draw method, which handles the drawing of the screen.
   * @param update A function pointer to the update method, which handles updating the screen's state.
   * @param onEnter A function pointer to the onEnter method, which is called when entering the screen.
   * @param onExit A function pointer to the onExit method, which is called when exiting the screen.
   */
  Screen(const char *name, void (*draw)(), void (*update)(), void (*onEnter)(), void (*onExit)());
  const char *name;
  void (*draw)();
  void (*update)();
  void (*onEnter)();
  void (*onExit)();
};

/**
 * @class LCD
 * @brief Manages an LCD screen using the LiquidCrystal_I2C library.
 *
 * The LCD class provides methods to initialize the LCD, update its state in a loop,
 * and switch between different screens. It maintains a pointer to the current screen
 * being displayed.
 */
class LCD
{
  /**
   * @brief Pointer to the current screen being displayed.
   *
   * This pointer holds the reference to the current Screen object that is
   * being displayed on the LCD. It is initialized to nullptr and should be
   * set to a valid Screen object before use.
   */
  Screen *currentScreen = nullptr;

  /**
   * @brief Stores the timestamp of the last draw operation.
   *
   * This variable holds the time in milliseconds when the last
   * draw operation was performed. It is used to determine if a redraw is necessary
   * based on the elapsed time.
   */
  uint64_t lastDraw = 0;

public:
  /**
   * @brief An instance of the LiquidCrystal_I2C class used to control an LCD display via the I2C protocol.
   *
   * This object allows for communication and control of an LCD display that uses the I2C interface.
   * It provides methods to initialize the display, write text, and control various display settings.
   */
  LiquidCrystal_I2C lcd;

  /**
   * @brief Constructor for the LCD class.
   */
  LCD();

  /**
   * @brief Initializes the LCD display.
   *
   * This function sets up the necessary configurations and initializes
   * the LCD display for use. It should be called before any other
   * operations are performed on the LCD.
   */
  void init();
  /**
   * @brief Main loop function that handles the repetitive tasks.
   *
   * This function is called continuously in the main program loop to
   * perform tasks such as updating the display, handling user input,
   * and managing other periodic activities.
   */
  void loop();

  /**
   * @brief Sets the current screen to the specified screen.
   *
   * This function changes the display to the provided screen object.
   *
   * @param screen A pointer to the Screen object to be displayed.
   */
  void setScreen(Screen *screen);
};

extern LCD lcd;