#pragma once

#include "Display.h"

#include <vector>

#define MENU_DEFUALT_CLICKS 1

class Menu;

enum class MenuItemType
{
  None,
  Action,
  Navigate,
  Back,
  Toggle,
  String,
  Number,
  Select,
  Submenu
};

enum class MenuSize
{
  Large,
  Medium,
  Small
};

struct ActionFunction
{
  std::function<void()> func;
  int8_t clicksToRun;
};

class MenuItem
{
  friend class Menu;

private:
  String name;

protected:
  Menu *parent;
  MenuItemType type;
  bool hidden = false;

  u16_t textColor;
  u16_t activeTextColor;
  u16_t bgColor;

public:
  std::vector<ActionFunction> functions;
  MenuItem(String _name);

  void addFunc(int8_t _clicksToRun, std::function<void()> _func);
  void setName(String _name);
  String getName();
  MenuItemType getType();
  void setTextColor(u16_t _color);
  void setActiveTextColor(u16_t _color);
  void setBgColor(u16_t _color);
  void setHidden(bool _hidden);
  bool isHidden();

  void executeFunc();
  void executeFunc(int8_t _clicks);

  virtual void draw(uint8_t _x, uint8_t _y, bool _active);
  virtual void run();
};

class MenuItemAction : public MenuItem
{
public:
  MenuItemAction(String _name, int8_t _clicksToRun, std::function<void()> _func);
};

class MenuItemNavigate : public MenuItem
{
private:
  String target;

public:
  MenuItemNavigate(String _name, String _target);
  void addRoute(int8_t _clicksToRun, String _target);
};

class MenuItemBack : public MenuItem
{
public:
  MenuItemBack();
};

class MenuItemToggle : public MenuItem
{
private:
  bool *value;
  bool isMutable = false;
  std::function<void()> onChange;

public:
  MenuItemToggle(String _name, bool *_value, bool _isMutable = true);

  void setOnChange(std::function<void()> _onChange);
  void removeOnChange();

  void set(bool _value);
  bool get();

  void draw(uint8_t _x, uint8_t _y, bool _active) override;
};

class MenuItemString : public MenuItem
{
private:
  String *value;

public:
  MenuItemString(String _name, String *_value);

  void draw(uint8_t _x, uint8_t _y, bool _active) override;
  void setValue(String _value);
  String getValue();
};

enum class NumberValueType
{
  INT,
  FLOAT,
  LONG,
  UINT8_T,
  UINT32_T,
  UNKNOWN
};

class MenuItemNumberBase : public MenuItem
{
protected:
  bool selected = false;
  bool isMutable = false;
  bool fastUpdate = false;

public:
  MenuItemNumberBase(String _name);

  virtual ~MenuItemNumberBase() = default;

  bool isSelected() const;
  virtual void increase() = 0;
  virtual void decrease() = 0;
  virtual NumberValueType getValueType() const = 0;

  void setFastUpdate(bool _fastUpdate);
  bool isFastUpdate();
};

template <typename T>
class MenuItemNumber : public MenuItemNumberBase
{
private:
  T *value;
  T min;
  T max;
  T step = 1;

  NumberValueType valueType;

  std::function<void()> onChange;

  void determineValueType();

public:
  MenuItemNumber(String _name, T *_value, T _min, T _max);
  MenuItemNumber(String _name, T *_value, T _min, T _max, T _step);
  MenuItemNumber(String _name, T *_value);

  void toggleSelected();
  virtual void increase() override;
  virtual void decrease() override;
  virtual NumberValueType getValueType() const override;
  virtual void draw(uint8_t _x, uint8_t _y, bool _active) override;

  void setOnChange(std::function<void()> _onChange);
  void removeOnChange();
};

template class MenuItemNumber<int>;
template class MenuItemNumber<float>;
template class MenuItemNumber<long>;
template class MenuItemNumber<uint8_t>;
template class MenuItemNumber<uint32_t>;
template class MenuItemNumber<uint64_t>;

class MenuItemSubmenu : public MenuItem
{
private:
  Menu *submenu;

public:
  MenuItemSubmenu(String _name, Menu *_submenu);
  virtual void run() override;
};

class MenuItemSelect : public MenuItem
{
private:
  std::vector<String> options;
  int currentIndex = 0;
  bool selected = false;
  std::function<void()> onChange;

public:
  MenuItemSelect(String _name, const std::vector<String> &_options, int initialIndex = 0);
  void nextOption();
  void prevOption();
  String getSelectedOption() const;
  void setCurrentIndex(int _index);
  int getCurrentIndex() const;
  void setOnChange(std::function<void()> callback);
  void toggleSelected();
  bool isSelected() const;
  virtual void run() override;
  virtual void draw(uint8_t _x, uint8_t _y, bool _active) override;
};

class Menu
{
  friend class MenuItem;

private:
  uint8_t active;
  uint8_t maxItemsPerPage = 0;
  uint8_t numItems;
  uint8_t numItemsPerPage;
  uint8_t offsetFromTop;
  uint8_t topItem = 0;

  MenuSize menuSize = MenuSize::Large;
  Menu *activeSubmenu = nullptr;
  Menu *parentMenu = nullptr;

public:
  std::vector<MenuItem *> items;
  Menu();
  Menu(MenuSize _size);

  void setItemsPerPage(uint8_t _itemsPerPage);
  uint8_t getItemsPerPage();

  void setMenuSize(MenuSize _size);
  MenuSize getMenuSize() const;

  void setActive(uint8_t _active);
  uint8_t getActive();

  void nextItem();
  void prevItem();

  void setActiveSubmenu(Menu *submenu);
  void clearActiveSubmenu();
  Menu *getActiveSubmenu();
  void setParentMenu(Menu *parent);
  Menu *getParentMenu();

  void addMenuItem(MenuItem *_item);
  void draw();
  void update();
};
