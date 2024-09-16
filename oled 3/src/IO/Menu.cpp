#include "Menu.h"

static const char *TAG = "Menu";

// ###### MenuItem ######

MenuItem::MenuItem(String _name)
{
  type = MenuItemType::None;
  name = _name;

  textColor = TFT_WHITE;
  activeTextColor = TFT_BLACK;
  bgColor = TFT_WHITE;
}

void MenuItem::addFunc(int8_t _clicksToRun, std::function<void()> _func)
{
  if (_clicksToRun == 0)
    return;

  ActionFunction actionFunc;
  actionFunc.clicksToRunUp = _clicksToRun;
  actionFunc.clicksToRunDown = _clicksToRun;
  actionFunc.func = _func;

  functions.push_back(actionFunc);
}

void MenuItem::addFunc(int8_t _clicksToRunUp, int8_t _clicksToRunDown, std::function<void()> _func)
{
  if (_clicksToRunUp == 0 && _clicksToRunDown == 0)
    return;

  ActionFunction actionFunc;
  actionFunc.clicksToRunUp = _clicksToRunUp;
  actionFunc.clicksToRunDown = _clicksToRunDown;
  actionFunc.func = _func;

  functions.push_back(actionFunc);
}

void MenuItem::setName(String _name)
{
  name = _name;
}

String MenuItem::getName()
{
  return name;
}

MenuItemType MenuItem::getType()
{
  return type;
}

void MenuItem::setHidden(bool _hidden)
{
  hidden = _hidden;
}

void MenuItem::setTextColor(u16_t _color)
{
  textColor = _color;
}

void MenuItem::setActiveTextColor(u16_t _color)
{
  activeTextColor = _color;
}

void MenuItem::setBgColor(u16_t _color)
{
  bgColor = _color;
}

bool MenuItem::isHidden()
{
  return hidden;
}

// ###### MenuItemAction ######

MenuItemAction::MenuItemAction(String _name, int8_t _clicksToRun, std::function<void()> _func) : MenuItem(_name)
{
  type = MenuItemType::Action;

  addFunc(_clicksToRun, _func);
}

// ###### MenuItemNavigate ######

MenuItemNavigate::MenuItemNavigate(String _name, String _target) : MenuItem(_name)
{
  type = MenuItemType::Navigate;
  target = _target;

  addFunc(2, [this]()
          { screenManager.setScreen(target); });
}

void MenuItemNavigate::addRoute(int8_t _clicksToRun, String _target)
{
  addFunc(_clicksToRun, [this, _target]()
          { screenManager.setScreen(_target); });
}

// ###### MenuItemBack ######

MenuItemBack::MenuItemBack() : MenuItem("Back")
{
  type = MenuItemType::Back;

  addFunc(2, []()
          { screenManager.back(); });
}

// ###### MenuItemToggle ######

MenuItemToggle::MenuItemToggle(String _name, bool *_value, bool _isMutable) : MenuItem(_name)
{
  type = MenuItemType::Toggle;
  value = _value;
  isMutable = _isMutable;

  addFunc(2, [this]()
          {
            if (isMutable)
              *value = !*value;
            if (onChange != nullptr)
              onChange();
            //
          });
}

void MenuItemToggle::setOnChange(std::function<void()> _onChange)
{
  onChange = _onChange;
}

void MenuItemToggle::removeOnChange()
{
  onChange = nullptr;
}

void MenuItemToggle::set(bool _value)
{
  if (isMutable)
    *value = _value;
}

bool MenuItemToggle::get()
{
  return *value;
}

// ###### MenuItemNumber ######

template <typename T>
MenuItemNumber<T>::MenuItemNumber(String _name, T *_value, T _min, T _max) : MenuItem(_name)
{
  type = MenuItemType::Number;

  value = _value;
  min = _min;
  max = _max;

  isMutable = true;

  addFunc(2, [this]()
          {
            selected = !selected;
            if (onChange != nullptr)
              onChange();
            //
          });
}

template <typename T>
MenuItemNumber<T>::MenuItemNumber(String _name, T *_value, T _min, T _max, T _step) : MenuItem(_name)
{
  type = MenuItemType::Number;

  value = _value;
  min = _min;
  max = _max;
  step = _step;

  isMutable = true;

  addFunc(2, [this]()
          {
            selected = !selected;
            if (onChange != nullptr)
              onChange();
            //
          });
}

template <typename T>
MenuItemNumber<T>::MenuItemNumber(String _name, T *_value) : MenuItem(_name)
{
  type = MenuItemType::Number;

  value = _value;
  min = 0;
  max = 0;

  isMutable = false;
}

template <typename T>
void MenuItemNumber<T>::setOnChange(std::function<void()> _onChange)
{
  onChange = _onChange;
}

template <typename T>
void MenuItemNumber<T>::removeOnChange()
{
  onChange = nullptr;
}

template <typename T>
bool MenuItemNumber<T>::isSelected()
{
  return selected;
}

template <typename T>
void MenuItemNumber<T>::increase()
{
  if (isMutable && selected)
    if (*value < max)
      *value += step;
}

template <typename T>
void MenuItemNumber<T>::decrease()
{
  if (isMutable && selected)
    if (*value > min)
      *value -= step;
}

// ###### Menu ######

Menu::Menu()
{
  // name = _name;
  active = 0;
}

void Menu::setItemsPerPage(uint8_t _itemsPerPage)
{
  maxItemsPerPage = _itemsPerPage;
}

uint8_t Menu::getItemsPerPage()
{
  return maxItemsPerPage;
}

void Menu::setActive(uint8_t _active)
{
  active = _active;
}

uint8_t Menu::getActive()
{
  return active;
}

void Menu::nextItem()
{
  if (active < numItems - 1)
  {
    do
    {
      active++;
    } while (items[active]->isHidden());
  }
}

void Menu::prevItem()
{
  if (active > 0)
  {
    do
    {
      active--;
    } while (items[active]->isHidden());
  }
}

void Menu::addMenuItem(MenuItem *_item)
{
  items.push_back(_item);

  numItems = items.size();
  numItemsPerPage = numItems < maxItemsPerPage ? numItems : maxItemsPerPage;
}
