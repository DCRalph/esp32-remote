// #pragma once

// #include "config.h"
// #include "Display.h"
// #include "Buttons.h"

// #include <vector>

// class Menu;

// enum class MenuItemType
// {
//   None,
//   Action,
//   Navigate,
//   Back,
//   Toggle,
//   Number
// };

// struct ActionFunction
// {
//   std::function<void()> func;
//   s8_t clicksToRunUp;
//   s8_t clicksToRunDown;
// };

// /**
//  * @class MenuItem
//  * @brief Represents a menu item.
//  */
// class MenuItem
// {
// private:
//   String name;
//   std::vector<ActionFunction> functions;

// protected:
//   MenuItemType type;

//   u16_t textColor;
//   u16_t activeTextColor;
//   u16_t bgColor;

// public:
//   /**
//    * @brief Constructs a MenuItem object with the specified name.
//    * @param _name The name of the menu item.
//    */
//   MenuItem(String _name);

//   /**
//    * @brief Sets the action to be performed when the menu item is selected.
//    *
//    * @param _clicksToRun The number of clicks required to run the action.
//    * @param _func The action to be performed.
//    */
//   void addFunc(s8_t _clicksToRun, std::function<void()> _func);

//   /**
//    * @brief Sets the action to be performed when the menu item is selected.
//    *
//    * @param _clicksToRunUp The number of clicks required to run the action when the UP button is pressed.
//    * @param _clicksToRunDown The number of clicks required to run the action when the DOWN button is pressed.
//    * @param _func The action to be performed.
//    */
//   void addFunc(s8_t _clicksToRunUp, s8_t _clicksToRunDown, std::function<void()> _func);

//   /**
//    * @brief Sets the name of the menu item.
//    * @param _name The name of the menu item.
//    */
//   void setName(String _name);

//   /**
//    * @brief Gets the name of the menu item.
//    * @return The name of the menu item.
//    */
//   String getName();

//   /**
//    * @brief Gets the type of the menu item.
//    * @return The type of the menu item.
//    */
//   MenuItemType getType();

//   /**
//    * @brief Sets the text color of the menu item.
//    * @param _color The text color.
//    */
//   void setTextColor(u16_t _color);

//   /**
//    * @brief Sets the active text color of the menu item.
//    * @param _color The active text color.
//    */
//   void setActiveTextColor(u16_t _color);

//   /**
//    * @brief Sets the background color of the menu item.
//    * @param _color The background color.
//    */
//   void setBgColor(u16_t _color);

//   /**
//    * @brief Draws the menu item on the screen.
//    * @param _x The x-coordinate of the menu item.
//    * @param _y The y-coordinate of the menu item.
//    * @param _active Indicates whether the menu item is active or not.
//    */
//   virtual void draw(u8_t _x, u8_t _y, bool _active);

//   /**
//    * @brief Runs the menu item.
//    */
//   virtual void run();
// };

// /**
//  * @class MenuItemAction
//  * @brief A class representing a menu item with an associated action.
//  *
//  * This class inherits from the base class MenuItem and adds a function pointer
//  * to store the action to be performed when the menu item is selected.
//  */
// class MenuItemAction : public MenuItem
// {
// public:
//   /**
//    * @brief Constructs a MenuItemAction object with the specified name and action.
//    *
//    * @param _name The name of the menu item.
//    * @param _clicksToRun The number of clicks required to run the action.
//    * @param _func The action to be performed when the menu item is selected.
//    */
//   MenuItemAction(String _name, s8_t _clicksToRun, std::function<void()> _func);

//   // void draw(u8_t _x, u8_t _y, bool _active) override;
// };
// class MenuItemNavigate : public MenuItem
// {
// private:
//   String target;

// public:
//   /**
//    * @brief Constructs a new MenuItemNavigate object.
//    *
//    * @param _name The name of the menu item.
//    * @param _target The target of the navigation.
//    */
//   MenuItemNavigate(String _name, String _target);

//   /**
//    * Adds a new route to the menu.
//    *
//    * @param route The route to be added.
//    */
//   void addRoute(s8_t _clicksToRun, String _target);

//   // void draw(u8_t _x, u8_t _y, bool _active) override;
// };

// /**
//  * @brief Represents a menu item that allows the user to go back to the previous menu.
//  */
// class MenuItemBack : public MenuItem
// {
// public:
//   /**
//    * @brief Constructs a new MenuItemBack object.
//    */
//   MenuItemBack();

//   // void draw(u8_t _x, u8_t _y, bool _active) override;
// };

// /**
//  * @brief Represents a toggle menu item.
//  *
//  * This class inherits from the MenuItem class and provides functionality for a toggle menu item.
//  * It allows the user to toggle a boolean value associated with the menu item.
//  */
// class MenuItemToggle : public MenuItem
// {
// private:
//   bool *value;

// public:
//   /**
//    * @brief Constructs a new MenuItemToggle object.
//    *
//    * @param _name The name of the menu item.
//    * @param _value A pointer to the boolean value associated with the menu item.
//    */
//   MenuItemToggle(String _name, bool *_value);

//   void draw(u8_t _x, u8_t _y, bool _active) override;
// };

// /**
//  * @brief Represents a menu item for selecting a number value.
//  */
// class MenuItemNumber : public MenuItem
// {
// private:
//   long *value; ///< Pointer to the value being controlled by this menu item.
//   s16_t min;   ///< The minimum value allowed.
//   s16_t max;   ///< The maximum value allowed.

//   bool isMutable = false; ///< Flag indicating whether this menu item is currently selected.

//   bool selected = false; ///< Flag indicating whether this menu item is currently selected.

// public:
//   /**
//    * @brief Constructs a new MenuItemNumber object.
//    *
//    * @param _name The name of the menu item.
//    * @param _value Pointer to the value being controlled by this menu item.
//    * @param _min The minimum value allowed.
//    * @param _max The maximum value allowed.
//    */
//   MenuItemNumber(String _name, long *_value, s16_t _min, s16_t _max);

//   /**
//    * @brief Constructs a new MenuItemNumber object.
//    *
//    * @param _name The name of the menu item.
//    * @param _value Pointer to the value being controlled by this menu item.
//    */
//   MenuItemNumber(String _name, long *_value);

//   /**
//    * @brief Checks if this menu item is currently selected.
//    *
//    * @return true if this menu item is selected, false otherwise.
//    */
//   bool isSelected();

//   /**
//    * @brief Increases the value by one.
//    */
//   void increase();

//   /**
//    * @brief Decreases the value by one.
//    */
//   void decrease();

//   void draw(u8_t _x, u8_t _y, bool _active) override;
// };

// // ###### Menu ######
// class Menu
// {
// private:
//   u8_t active;
//   std::vector<MenuItem *> items;

//   u8_t maxItemsPerPage = 5;

//   u8_t offsetFromTop;
//   u8_t numItems;
//   u8_t numItemsPerPage;

// public:
//   Menu();

//   // String name;

//   void setItemsPerPage(u8_t _itemsPerPage);
//   u8_t getItemsPerPage();

//   void setActive(u8_t _active);
//   u8_t getActive();

//   void nextItem();
//   void prevItem();

//   void addMenuItem(MenuItem *_item);
//   void draw();
//   void update();
// };
