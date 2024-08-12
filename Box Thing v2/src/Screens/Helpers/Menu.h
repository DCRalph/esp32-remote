#pragma once

#include "config.h"
#include <vector>

#include "IO/Display.h"
#include "IO/GPIO.h"

class Menu;

enum class MenuItemType
{
  None,
  Action,
  Navigate,
  Back,
  Toggle,
  Number
};

struct ActionFunction
{
  std::function<void()> func;
  int8_t clicksToRun;
};

/**
 * @class MenuItem
 * @brief Represents a menu item.
 */
class MenuItem
{
private:
  String name;
  std::vector<ActionFunction> functions;

protected:
  MenuItemType type;
  bool hidden = false;

public:
  /**
   * @brief Constructs a MenuItem object with the specified name.
   * @param _name The name of the menu item.
   */
  MenuItem(String _name);

  /**
   * @brief Sets the action to be performed when the menu item is selected.
   *
   * @param _clicksToRun The number of clicks required to run the action.
   * @param _func The action to be performed.
   */
  void addFunc(int8_t _clicksToRun, std::function<void()> _func);

  /**
   * @brief Sets the name of the menu item.
   * @param _name The name of the menu item.
   */
  void setName(String _name);

  /**
   * @brief Gets the name of the menu item.
   * @return The name of the menu item.
   */
  String getName();

  /**
   * @brief Gets the type of the menu item.
   * @return The type of the menu item.
   */
  MenuItemType getType();

  /**
   * @brief Sets the visibility of the menu item.
   * @param _hidden Indicates whether the menu item is hidden or not.
   */
  void setHidden(bool _hidden);

  /**
   * @brief Checks if the menu item is hidden.
   * @return true if the menu item is hidden, false otherwise.
   */
  bool isHidden();

  /**
   * @brief Draws the menu item on the screen.
   * @param _x The x-coordinate of the menu item.
   * @param _y The y-coordinate of the menu item.
   * @param _active Indicates whether the menu item is active or not.
   */
  virtual void draw(uint8_t _x, uint8_t _y, bool _active);

  /**
   * @brief Runs the menu item.
   */
  virtual void run();
};

/**
 * @class MenuItemAction
 * @brief A class representing a menu item with an associated action.
 *
 * This class inherits from the base class MenuItem and adds a function pointer
 * to store the action to be performed when the menu item is selected.
 */
class MenuItemAction : public MenuItem
{
public:
  /**
   * @brief Constructs a MenuItemAction object with the specified name and action.
   *
   * @param _name The name of the menu item.
   * @param _clicksToRun The number of clicks required to run the action.
   * @param _func The action to be performed when the menu item is selected.
   */
  MenuItemAction(String _name, int8_t _clicksToRun, std::function<void()> _func);

  // void draw(u8 _x, u8 _y, bool _active) override;
};
class MenuItemNavigate : public MenuItem
{
private:
  String target;

public:
  /**
   * @brief Constructs a new MenuItemNavigate object.
   *
   * @param _name The name of the menu item.
   * @param _target The target of the navigation.
   */
  MenuItemNavigate(String _name, String _target);

  /**
   * Adds a new route to the menu.
   *
   * @param route The route to be added.
   */
  void addRoute(int8_t _clicksToRun, String _target);

  // void draw(u8 _x, u8 _y, bool _active) override;
};

/**
 * @brief Represents a menu item that allows the user to go back to the previous menu.
 */
class MenuItemBack : public MenuItem
{
public:
  /**
   * @brief Constructs a new MenuItemBack object.
   */
  MenuItemBack();

  // void draw(u8 _x, u8 _y, bool _active) override;
};

/**
 * @brief Represents a toggle menu item.
 *
 * This class inherits from the MenuItem class and provides functionality for a toggle menu item.
 * It allows the user to toggle a boolean value associated with the menu item.
 */
class MenuItemToggle : public MenuItem
{
private:
  bool *value; ///< Pointer to the boolean value associated with the menu item.
  bool isMutable = false; ///< Flag indicating whether this menu item is currently selected.
  std::function<void()> onChange;

public:
  /**
   * @brief Constructs a new MenuItemToggle object.
   *
   * @param _name The name of the menu item.
   * @param _value A pointer to the boolean value associated with the menu item.
   */
  MenuItemToggle(String _name, bool *_value, bool _isMutable = true);

  void setOnChange(std::function<void()> _onChange);
  void removeOnChange();

  void set(bool _value);
  bool get();

  void draw(uint8_t _x, uint8_t _y, bool _active) override;
};

/**
 * @brief Represents a menu item for selecting a number value.
 */
template <typename T>
class MenuItemNumber : public MenuItem
{
private:
  T *value; ///< Pointer to the value being controlled by this menu item.
  int min;  ///< The minimum value allowed.
  int max;  ///< The maximum value allowed.

  bool isMutable = false; ///< Flag indicating whether this menu item is currently selected.

  bool selected = false; ///< Flag indicating whether this menu item is currently selected.

  std::function<void()> onChange;

public:
  /**
   * @brief Constructs a new MenuItemNumber object.
   *
   * @param _name The name of the menu item.
   * @param _value Pointer to the value being controlled by this menu item.
   * @param _min The minimum value allowed.
   * @param _max The maximum value allowed.
   */
  MenuItemNumber(String _name, T *_value, int _min, int _max);

  void setOnChange(std::function<void()> _onChange);
  void removeOnChange();

  /**
   * @brief Constructs a new MenuItemNumber object.
   *
   * @param _name The name of the menu item.
   * @param _value Pointer to the value being controlled by this menu item.
   */
  MenuItemNumber(String _name, T *_value);

  /**
   * @brief Checks if this menu item is currently selected.
   *
   * @return true if this menu item is selected, false otherwise.
   */
  bool isSelected();

  /**
   * @brief Increases the value by one.
   */
  void increase();

  /**
   * @brief Decreases the value by one.
   */
  void decrease();

  void draw(uint8_t _x, uint8_t _y, bool _active) override;
};

template class MenuItemNumber<int>;
template class MenuItemNumber<long>;

// ###### Menu ######
class Menu
{
private:
  uint8_t active;
  std::vector<MenuItem *> items;

  uint8_t maxItemsPerPage = 3;

  uint8_t numItems;
  uint8_t numItemsPerPage;

public:
  Menu();

  // String name;

  void setItemsPerPage(uint8_t _itemsPerPage);
  uint8_t getItemsPerPage();

  void setActive(uint8_t _active);
  uint8_t getActive();

  void nextItem();
  void prevItem();

  void addMenuItem(MenuItem *_item);
  void draw();
  void update();
};
