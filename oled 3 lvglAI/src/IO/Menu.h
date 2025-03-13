#pragma once

#include "config.h"
#include <lvgl.h>
#include <vector>
#include <functional>
#include <string>

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
  int8_t clicksToRunUp;
  int8_t clicksToRunDown;
};

class MenuItem
{
private:
  std::string name;

protected:
  MenuItemType type;
  bool hidden = false;
  lv_color_t textColor;
  lv_color_t activeTextColor;
  lv_color_t bgColor;

public:
  std::vector<ActionFunction> functions;

  MenuItem(const std::string &_name);
  virtual ~MenuItem() = default;

  void addFunc(int8_t _clicksToRun, std::function<void()> _func);
  void addFunc(int8_t _clicksToRunUp, int8_t _clicksToRunDown, std::function<void()> _func);
  void setName(const std::string &_name);
  std::string getName() const;
  MenuItemType getType() const;
  void setTextColor(lv_color_t _color);
  void setActiveTextColor(lv_color_t _color);
  void setBgColor(lv_color_t _color);
  void setHidden(bool _hidden);
  bool isHidden() const;
  void executeFunc();
  void executeFunc(int8_t _clicks);

  virtual lv_obj_t *createLVGLObject(lv_obj_t *parent, lv_group_t *group) = 0;
};

class MenuItemAction : public MenuItem
{
public:
  MenuItemAction(const std::string &_name, int8_t _clicksToRun, std::function<void()> _func);
  lv_obj_t *createLVGLObject(lv_obj_t *parent, lv_group_t *group) override;
};

class MenuItemNavigate : public MenuItem
{
private:
  std::string target;

public:
  MenuItemNavigate(const std::string &_name, const std::string &_target);
  void addRoute(int8_t _clicksToRun, const std::string &_target);
  lv_obj_t *createLVGLObject(lv_obj_t *parent, lv_group_t *group) override;
};

class MenuItemBack : public MenuItem
{
public:
  MenuItemBack();
  lv_obj_t *createLVGLObject(lv_obj_t *parent, lv_group_t *group) override;
};

class MenuItemToggle : public MenuItem
{
private:
  bool *value;
  bool isMutable = false;
  std::function<void()> onChange;

public:
  MenuItemToggle(const std::string &_name, bool *_value, bool _isMutable = true);
  void setOnChange(std::function<void()> _onChange);
  void removeOnChange();
  void set(bool _value);
  bool get() const;
  lv_obj_t *createLVGLObject(lv_obj_t *parent, lv_group_t *group) override;
};

template <typename T>
class MenuItemNumber : public MenuItem
{
private:
  T *value;
  T min;
  T max;
  T step = 1;
  bool isMutable = false;
  bool selected = false;
  std::function<void()> onChange;

public:
  MenuItemNumber(const std::string &_name, T *_value, T _min, T _max);
  MenuItemNumber(const std::string &_name, T *_value, T _min, T _max, T _step);
  MenuItemNumber(const std::string &_name, T *_value);

  void setOnChange(std::function<void()> _onChange);
  void removeOnChange();
  bool isSelected() const;
  void increase();
  void decrease();
  lv_obj_t *createLVGLObject(lv_obj_t *parent, lv_group_t *group) override;
};

template class MenuItemNumber<int>;
template class MenuItemNumber<long>;
template class MenuItemNumber<uint8_t>;
template class MenuItemNumber<uint32_t>;

class Menu
{
private:
  uint8_t active;
  uint8_t maxItemsPerPage = 5;
  uint8_t numItems;
  uint8_t numItemsPerPage;
  uint8_t offsetFromTop;
  lv_obj_t *list;
  lv_group_t *navGroup;

public:
  std::vector<MenuItem *> items;
  Menu(lv_obj_t *parent, lv_group_t *group = nullptr);
  ~Menu();

  void setItemsPerPage(uint8_t _itemsPerPage);
  uint8_t getItemsPerPage() const;
  void setActive(uint8_t _active);
  uint8_t getActive() const;
  void nextItem();
  void prevItem();
  void addMenuItem(MenuItem *_item);
  void refresh();
  lv_obj_t *getList() const { return list; }
};
