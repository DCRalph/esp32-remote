#include "Menu.h"

// ###### MenuItem ######

MenuItem::MenuItem(String _name)
{
  type = MenuItemType::None;
  name = _name;
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

void MenuItem::draw(uint8_t _x, uint8_t _y, bool _active)
{
  display.u8g2.setFont(u8g2_font_profont22_tf);

  if (_active)
  {
    display.u8g2.setDrawColor(1);
    display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 16);
    display.u8g2.setDrawColor(0);
  }
  else
    display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(_x + 1, _y + 15, getName().c_str());
}

void MenuItem::run()
{
  for (uint8_t i = 0; i < functions.size(); i++)
  {
    if (ClickButtonEnc.clicks == functions[i].clicksToRun)
    {
      functions[i].func();
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

  addFunc(1, [this]()
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

  addFunc(1, []()
          { screenManager.back(); });
}

// ###### MenuItemToggle ######

MenuItemToggle::MenuItemToggle(String _name, bool *_value) : MenuItem(_name)
{
  type = MenuItemType::Toggle;
  value = _value;

  addFunc(1, [this]()
          {
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

void MenuItemToggle::draw(uint8_t _x, uint8_t _y, bool _active)
{
  display.u8g2.setFont(u8g2_font_profont22_tf);

  if (_active)
  {
    display.u8g2.setDrawColor(1);
    display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 16);
    display.u8g2.setDrawColor(0);
  }
  else
    display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(_x + 1, _y + 15, getName().c_str());

  String valueStr = *value ? "ON" : "OFF";
  display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 15, valueStr.c_str());
}

// ###### MenuItemNumber ######

MenuItemNumber::MenuItemNumber(String _name, long *_value, int _min, int _max) : MenuItem(_name)
{
  type = MenuItemType::Number;

  value = _value;
  min = _min;
  max = _max;

  isMutable = true;

  addFunc(1, [this]()
          { selected = !selected; });
}

MenuItemNumber::MenuItemNumber(String _name, long *_value) : MenuItem(_name)
{
  type = MenuItemType::Number;

  value = _value;
  min = 0;
  max = 0;

  isMutable = false;
}

bool MenuItemNumber::isSelected()
{
  return selected;
}

void MenuItemNumber::increase()
{
  if (isMutable && selected)
    if (*value < max)
      *value += 1;
}

void MenuItemNumber::decrease()
{
  if (isMutable && selected)
    if (*value > min)
      *value -= 1;
}

void MenuItemNumber::draw(uint8_t _x, uint8_t _y, bool _active)
{
  display.u8g2.setFont(u8g2_font_profont22_tf);

  if (!_active && selected)
  {
    selected = false;
  }

  String valueStr = String(*value);

  if (_active && selected)
  {
    display.u8g2.setDrawColor(1);
    display.u8g2.drawFrame(_x, _y, DISPLAY_WIDTH - 4, 16);
    display.u8g2.drawStr(_x + 1, _y + 15, getName().c_str());
    display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 15, valueStr.c_str());
  }
  else if (_active)
  {
    display.u8g2.setDrawColor(1);
    display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 16);
    display.u8g2.setDrawColor(0);
    display.u8g2.drawStr(_x + 1, _y + 15, getName().c_str());
    display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 15, valueStr.c_str());
  }
  else
  {
    display.u8g2.setDrawColor(1);
    display.u8g2.drawStr(_x + 1, _y + 15, getName().c_str());
    display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 15, valueStr.c_str());
  }
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
    active++;
}

void Menu::prevItem()
{
  if (active > 0)
    active--;
}

void Menu::addMenuItem(MenuItem *_item)
{
  items.push_back(_item);

  numItems = items.size();
  numItemsPerPage = numItems < maxItemsPerPage ? numItems : maxItemsPerPage;
}

void Menu::draw()
{
  display.u8g2.setFont(u8g2_font_profont22_tf);

  for (uint8_t i = 0; i < numItemsPerPage; i++)
  {
    uint8_t topItem;

    if (active - 1 < 0)
      topItem = 0;
    else if (active - 1 >= static_cast<u8_t>(numItems - 2))
      if (numItems < 3)
        topItem = 0;
      else
        topItem = static_cast<u8_t>(numItems - 3);
    else
      topItem = active - 1;

    MenuItem *item = items[i + topItem];

    item->draw(0, 12 + (i * 18), active == i + topItem);
  }

  display.u8g2.setDrawColor(1);

  uint8_t scrollBarPosition = (DISPLAY_HEIGHT - 13) / (numItems < 1 ? 1 : numItems) * active;
  uint8_t scrollBarHeight = (numItems < 1 ? 1 : numItems) - 1 == active ? DISPLAY_HEIGHT - 12 - scrollBarPosition : (DISPLAY_HEIGHT - 13) / (numItems < 1 ? 1 : numItems);

  display.u8g2.drawLine(DISPLAY_WIDTH - 2, 12, DISPLAY_WIDTH - 2, DISPLAY_HEIGHT - 1);
  display.u8g2.drawBox(DISPLAY_WIDTH - 3, 12 + scrollBarPosition, 3, scrollBarHeight);
}

void Menu::update()
{
  if (numItems < 1)
  {
    if (ClickButtonEnc.clicks == 1)
      screenManager.back();

    return;
  }

  if (ClickButtonEnc.clicks != 0)
    items[active]->run();

  if (encoderGetCount() > 0)
  {
    if (items[active]->getType() == MenuItemType::Number && ((MenuItemNumber *)items[active])->isSelected())
      ((MenuItemNumber *)items[active])->increase();
    else
      nextItem();
    encoderClearCount();
  }
  else if (encoderGetCount() < 0)
  {
    if (items[active]->getType() == MenuItemType::Number && ((MenuItemNumber *)items[active])->isSelected())
      ((MenuItemNumber *)items[active])->decrease();
    else
      prevItem();
    encoderClearCount();
  }
}