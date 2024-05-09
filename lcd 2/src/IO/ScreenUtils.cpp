#include "ScreenUtils.h"

// ###### MenuItem ######

MenuItem::MenuItem(String _name)
{
  type = MenuItemType::None;
  name = _name;
}

void MenuItem::addFunc(s8_t _clicksToRun, std::function<void()> _func)
{
  if (_clicksToRun == 0)
    return;

  ActionFunction actionFunc;
  actionFunc.clicksToRunUp = _clicksToRun;
  actionFunc.clicksToRunDown = _clicksToRun;
  actionFunc.func = _func;

  functions.push_back(actionFunc);
}

void MenuItem::addFunc(s8_t _clicksToRunUp, s8_t _clicksToRunDown, std::function<void()> _func)
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

void MenuItem::draw(u8_t _x, u8_t _y, bool _active)
{
  display.sprite.setTextSize(2);

  if (_active)
  {
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, TFT_WHITE);
    display.sprite.setTextColor(TFT_BLACK);
  }
  else
    display.sprite.setTextColor(TFT_WHITE);

  display.sprite.setTextDatum(TL_DATUM);
  display.sprite.drawString(getName(), _x + 4, _y + 3);
}

void MenuItem::run()
{
  for (u8_t i = 0; i < functions.size(); i++)
  {
    if (ClickButtonUP.clicks == functions[i].clicksToRunUp || ClickButtonDOWN.clicks == functions[i].clicksToRunDown)
    {
      functions[i].func();
      break;
    }
  }
}

// ###### MenuItemAction ######

MenuItemAction::MenuItemAction(String _name, s8_t _clicksToRun, std::function<void()> _func) : MenuItem(_name)
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

void MenuItemNavigate::addRoute(s8_t _clicksToRun, String _target)
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

MenuItemToggle::MenuItemToggle(String _name, bool *_value) : MenuItem(_name)
{
  type = MenuItemType::Toggle;
  value = _value;

  addFunc(2, [this]()
          { *value = !*value; });
}

void MenuItemToggle::draw(u8_t _x, u8_t _y, bool _active)
{
  display.sprite.setTextSize(2);

  if (_active)
  {
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, TFT_WHITE);
    display.sprite.setTextColor(TFT_BLACK);
  }
  else
    display.sprite.setTextColor(TFT_WHITE);

  display.sprite.setTextDatum(TL_DATUM);
  display.sprite.drawString(getName(), _x + 4, _y + 3);

  String valueStr = *value ? "ON" : "OFF";
  display.sprite.setTextDatum(TR_DATUM);
  display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 3);
}

// ###### MenuItemNumber ######

MenuItemNumber::MenuItemNumber(String _name, long *_value, s16_t _min, s16_t _max) : MenuItem(_name)
{
  type = MenuItemType::Number;

  value = _value;
  min = _min;
  max = _max;

  isMutable = true;

  addFunc(2, [this]()
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

void MenuItemNumber::draw(u8_t _x, u8_t _y, bool _active)
{
  // display.u8g2.setFont(u8g2_font_profont22_tf);

  // if (!_active && selected)
  // {
  //   selected = false;
  // }

  // String valueStr = String(*value);

  // if (_active && selected)
  // {
  //   display.u8g2.setDrawColor(1);
  //   display.u8g2.drawFrame(_x, _y, DISPLAY_WIDTH - 4, 16);
  //   display.u8g2.drawStr(_x + 1, _y + 15, getName().c_str());
  //   display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 15, valueStr.c_str());
  // }
  // else if (_active)
  // {
  //   display.u8g2.setDrawColor(1);
  //   display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 16);
  //   display.u8g2.setDrawColor(0);
  //   display.u8g2.drawStr(_x + 1, _y + 15, getName().c_str());
  //   display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 15, valueStr.c_str());
  // }
  // else
  // {
  //   display.u8g2.setDrawColor(1);
  //   display.u8g2.drawStr(_x + 1, _y + 15, getName().c_str());
  //   display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 15, valueStr.c_str());
  // }

  display.sprite.setTextSize(2);

  if (!_active && selected)
  {
    selected = false;
  }

  String valueStr = String(*value);

  if (_active && selected)
  {
    display.sprite.drawRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, TFT_WHITE);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.setTextDatum(TL_DATUM);
    display.sprite.drawString(getName(), _x + 4, _y + 2);
    display.sprite.setTextDatum(TR_DATUM);
    display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 2);
  }
  else if (_active)
  {
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, TFT_WHITE);
    display.sprite.setTextColor(TFT_BLACK);
    display.sprite.drawString(getName(), _x + 4, _y + 2);
    display.sprite.setTextDatum(TR_DATUM);
    display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 2);
  }
  else
  {
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString(getName(), _x + 4, _y + 2);
    display.sprite.setTextDatum(TR_DATUM);
    display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 2);
  }
}

// ###### Menu ######

Menu::Menu()
{
  active = 0;
  offsetFromTop = 0;
}

void Menu::setItemsPerPage(u8_t _itemsPerPage)
{
  maxItemsPerPage = _itemsPerPage;
}

u8_t Menu::getItemsPerPage()
{
  return maxItemsPerPage;
}

void Menu::setActive(u8_t _active)
{
  active = _active;
}

u8_t Menu::getActive()
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
  // display.sprite.setTextSize(1);
  // display.sprite.setTextColor(TFT_WHITE);
  // display.sprite.setTextDatum(TL_DATUM);

  // char buf[50];
  // sprintf(buf, "Menu (%d/%d)", active, offsetFromTop);

  // display.sprite.drawString(buf, 0, 0);

  display.sprite.setTextSize(2);

  if (active - offsetFromTop >= numItemsPerPage - 1 && active < numItems - 1)
    offsetFromTop = active - numItemsPerPage + 2;
  if (active <= offsetFromTop && active > 0)
    offsetFromTop = active - 1;

  for (u8_t i = 0; i < numItemsPerPage; i++)
  {
    MenuItem *item = items[i + offsetFromTop];

    item->draw(5, 25 + (i * 20), active == i + offsetFromTop);
  }

  u8_t scrollBarPosition = (LCD_HEIGHT - 31) / (numItems < 1 ? 1 : numItems) * active;
  u8_t scrollBarHeight = (numItems < 1 ? 1 : numItems) - 1 == active ? LCD_HEIGHT - 30 - scrollBarPosition : (LCD_HEIGHT - 31) / (numItems < 1 ? 1 : numItems);

  display.sprite.drawLine(LCD_WIDTH - 2, 30, LCD_WIDTH - 2, LCD_HEIGHT - 1, TFT_WHITE);
  display.sprite.fillRect(LCD_WIDTH - 3, 30 + scrollBarPosition, 3, scrollBarHeight, TFT_WHITE);
}

void Menu::update()
{
  if (numItems < 1)
  {
    if (ClickButtonUP.clicks == 1 || ClickButtonDOWN.clicks == 1)
      screenManager.back();

    return;
  }

  if (ClickButtonUP.clicks != 0 && ClickButtonUP.clicks != 1 || ClickButtonDOWN.clicks != 0 && ClickButtonDOWN.clicks != 1)
    items[active]->run();

  if (ClickButtonDOWN.clicks == 1)
  {
    if (items[active]->getType() == MenuItemType::Number && ((MenuItemNumber *)items[active])->isSelected())
      ((MenuItemNumber *)items[active])->decrease();
    else
      nextItem();
  }
  else if (ClickButtonUP.clicks == 1)
  {
    if (items[active]->getType() == MenuItemType::Number && ((MenuItemNumber *)items[active])->isSelected())
      ((MenuItemNumber *)items[active])->increase();
    else
      prevItem();
  }
}