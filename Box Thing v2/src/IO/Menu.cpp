#include "Menu.h"

static const char *TAG = "Menu";

// ###### MenuItem ######

MenuItem::MenuItem(String _name)
{
  type = MenuItemType::None;
  name = _name;

  // textColor = TFT_WHITE;
  // activeTextColor = TFT_BLACK;
  // bgColor = TFT_WHITE;
}

void MenuItem::addFunc(int8_t _clicksToRun, std::function<void()> _func)
{
  if (_clicksToRun == 0)
    return;

  ActionFunction actionFunc;
  actionFunc.clicksToRun = _clicksToRun;
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

void MenuItem::executeFunc()
{
  if (functions.size() > 0 && functions[0].clicksToRun == 1)
    functions[0].func();
}

void MenuItem::executeFunc(int8_t _clicks)
{
  for (ActionFunction &actionFunc : functions)
  {
    if (actionFunc.clicksToRun == _clicks)
    {
      actionFunc.func();
      break;
    }
  }
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

  addFunc(MENU_DEFUALT_CLICKS, [this]()
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

  addFunc(MENU_DEFUALT_CLICKS, []()
          { screenManager.back(); });
}

// ###### MenuItemToggle ######

MenuItemToggle::MenuItemToggle(String _name, bool *_value, bool _isMutable) : MenuItem(_name)
{
  type = MenuItemType::Toggle;
  value = _value;
  isMutable = _isMutable;

  addFunc(MENU_DEFUALT_CLICKS, [this]()
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

// ###### MenuItemNumberBase ######

MenuItemNumberBase::MenuItemNumberBase(String _name)
    : MenuItem(_name)
{
  type = MenuItemType::Number;
}

bool MenuItemNumberBase::isSelected() const
{
  return selected;
}

// ###### MenuItemNumber ######

template <typename T>
MenuItemNumber<T>::MenuItemNumber(String _name, T *_value, T _min, T _max)
    : MenuItemNumberBase(_name), value(_value), min(_min), max(_max)
{
  determineValueType();
  isMutable = true;

  addFunc(1, [this]()
          { toggleSelected(); });
}

template <typename T>
MenuItemNumber<T>::MenuItemNumber(String _name, T *_value, T _min, T _max, T _step)
    : MenuItemNumberBase(_name), value(_value), min(_min), max(_max), step(_step)
{
  determineValueType();
  isMutable = true;

  addFunc(1, [this]()
          { toggleSelected(); });
}

template <typename T>
MenuItemNumber<T>::MenuItemNumber(String _name, T *_value)
    : MenuItemNumberBase(_name), value(_value),
      min(std::numeric_limits<T>::lowest()), max(std::numeric_limits<T>::max())
{
  determineValueType();
  isMutable = false;
}

template <typename T>
void MenuItemNumber<T>::determineValueType()
{
  if (std::is_same<T, int>::value)
    valueType = NumberValueType::INT;
  else if (std::is_same<T, float>::value)
    valueType = NumberValueType::FLOAT;
  else if (std::is_same<T, long>::value)
    valueType = NumberValueType::LONG;
  else if (std::is_same<T, uint8_t>::value)
    valueType = NumberValueType::UINT8_T;
  else if (std::is_same<T, uint32_t>::value)
    valueType = NumberValueType::UINT32_T;
  else
    valueType = NumberValueType::UNKNOWN;
}

template <typename T>
NumberValueType MenuItemNumber<T>::getValueType() const
{
  return valueType;
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
void MenuItemNumber<T>::toggleSelected()
{
  if (isMutable)
  {
    selected = !selected;

    if (!selected && onChange != nullptr)
      onChange();
  }
  else
    selected = false;
}

template <typename T>
void MenuItemNumber<T>::increase()
{
  if (value)
  {
    int64_t newValue = static_cast<int64_t>(*value) + static_cast<int64_t>(step);
    if (newValue <= static_cast<int64_t>(max))
    {
      *value = static_cast<T>(newValue);
    }
  }
}

template <typename T>
void MenuItemNumber<T>::decrease()
{
  if (value)
  {
    int64_t newValue = static_cast<int64_t>(*value) - static_cast<int64_t>(step);
    if (newValue >= static_cast<int64_t>(min))
    {
      *value = static_cast<T>(newValue);
    }
  }
}

// ##############################
// Implementation for MenuItemSelect
// ##############################

MenuItemSelect::MenuItemSelect(String _name,
                               const std::vector<String> &_options,
                               int initialIndex)
    : MenuItem(_name), options(_options)
{
  type = MenuItemType::Select;
  if (initialIndex >= 0 && initialIndex < (int)options.size())
  {
    currentIndex = initialIndex;
  }
  else
  {
    currentIndex = 0;
  }

  // When pressed, toggle selection mode.
  addFunc(MENU_DEFUALT_CLICKS, [this]()
          { toggleSelected(); });
}

void MenuItemSelect::nextOption()
{
  if (options.empty())
    return;
  currentIndex = (currentIndex + 1) % options.size();
  if (onChange)
    onChange();
}

void MenuItemSelect::prevOption()
{
  if (options.empty())
    return;
  currentIndex = (currentIndex - 1 + options.size()) % options.size();
  if (onChange)
    onChange();
}

String MenuItemSelect::getSelectedOption() const
{
  if (options.empty())
    return "";
  return options[currentIndex];
}

int MenuItemSelect::getCurrentIndex() const
{
  return currentIndex;
}

void MenuItemSelect::setOnChange(std::function<void()> callback)
{
  onChange = callback;
}

void MenuItemSelect::toggleSelected()
{
  if (options.empty())
    return;
  selected = !selected;

  if (!selected && onChange)
    onChange();
}

bool MenuItemSelect::isSelected() const
{
  return selected;
}

void MenuItemSelect::run()
{
  // When the item is clicked, we simply toggle editing mode.
  // (The encoder events should now change the option if selected.)
  toggleSelected();
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
