#include "Menu.h"

// ###### MenuItem ######

MenuItem::MenuItem(String _name)
{
  type = MenuItemType::None;
  name = _name;
}

void MenuItem::addFunc(s8 _clicksToRun, std::function<void()> _func)
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

void MenuItem::draw(u8 _x, u8 _y, bool _active)
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
  for (u8 i = 0; i < functions.size(); i++)
  {
    if (ClickButtonEnc.clicks == functions[i].clicksToRun)
    {
      functions[i].func();
      break;
    }
  }
}

// ###### MenuItemAction ######

MenuItemAction::MenuItemAction(String _name, s8 _clicksToRun, std::function<void()> _func) : MenuItem(_name)
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

void MenuItemNavigate::addRoute(s8 _clicksToRun, String _target)
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
          { *value = !*value; });
}

void MenuItemToggle::draw(u8 _x, u8 _y, bool _active)
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

MenuItemNumber::MenuItemNumber(String _name, long *_value, s16 _min, s16 _max) : MenuItem(_name)
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

void MenuItemNumber::draw(u8 _x, u8 _y, bool _active)
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

void Menu::setItemsPerPage(u8 _itemsPerPage)
{
  maxItemsPerPage = _itemsPerPage;
}

u8 Menu::getItemsPerPage()
{
  return maxItemsPerPage;
}

void Menu::setActive(u8 _active)
{
  active = _active;
}

u8 Menu::getActive()
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

  for (u8 i = 0; i < numItemsPerPage; i++)
  {
    u8_t topItem;

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

  u8 scrollBarPosition = (DISPLAY_HEIGHT - 13) / (numItems < 1 ? 1 : numItems) * active;
  u8 scrollBarHeight = (numItems < 1 ? 1 : numItems) - 1 == active ? DISPLAY_HEIGHT - 12 - scrollBarPosition : (DISPLAY_HEIGHT - 13) / (numItems < 1 ? 1 : numItems);

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

  if (encoder.encoder.getPosition() > 0)
  {
    if (items[active]->getType() == MenuItemType::Number && ((MenuItemNumber *)items[active])->isSelected())
      ((MenuItemNumber *)items[active])->increase();
    else
      nextItem();
    encoder.encoder.setPosition(0);
  }
  else if (encoder.encoder.getPosition() < 0)
  {
    if (items[active]->getType() == MenuItemType::Number && ((MenuItemNumber *)items[active])->isSelected())
      ((MenuItemNumber *)items[active])->decrease();
    else
      prevItem();
    encoder.encoder.setPosition(0);
  }
}