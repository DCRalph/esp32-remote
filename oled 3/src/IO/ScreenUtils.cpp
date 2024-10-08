// #include "ScreenUtils.h"

// static const char *TAG = "ScreenUtils";

// // ###### MenuItem ######

// MenuItem::MenuItem(String _name)
// {
//   type = MenuItemType::None;
//   name = _name;

//   textColor = TFT_WHITE;
//   activeTextColor = TFT_BLACK;
//   bgColor = TFT_WHITE;
// }

// void MenuItem::addFunc(s8_t _clicksToRun, std::function<void()> _func)
// {
//   if (_clicksToRun == 0)
//     return;

//   ActionFunction actionFunc;
//   actionFunc.clicksToRunUp = _clicksToRun;
//   actionFunc.clicksToRunDown = _clicksToRun;
//   actionFunc.func = _func;

//   functions.push_back(actionFunc);
// }

// void MenuItem::addFunc(s8_t _clicksToRunUp, s8_t _clicksToRunDown, std::function<void()> _func)
// {
//   if (_clicksToRunUp == 0 && _clicksToRunDown == 0)
//     return;

//   ActionFunction actionFunc;
//   actionFunc.clicksToRunUp = _clicksToRunUp;
//   actionFunc.clicksToRunDown = _clicksToRunDown;
//   actionFunc.func = _func;

//   functions.push_back(actionFunc);
// }

// void MenuItem::setName(String _name)
// {
//   name = _name;
// }

// String MenuItem::getName()
// {
//   return name;
// }

// MenuItemType MenuItem::getType()
// {
//   return type;
// }

// void MenuItem::setTextColor(u16_t _color)
// {
//   textColor = _color;
// }

// void MenuItem::setActiveTextColor(u16_t _color)
// {
//   activeTextColor = _color;
// }

// void MenuItem::setBgColor(u16_t _color)
// {
//   bgColor = _color;
// }

// void MenuItem::draw(u8_t _x, u8_t _y, bool _active)
// {
//   display.sprite.setTextSize(2 * TFT_SCALE);

//   if (_active)
//   {
// #ifdef TFT_BIG
//     display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 15, 37, 10, bgColor);
// #else
//     display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, bgColor);
// #endif

//     display.sprite.setTextColor(activeTextColor);
//   }
//   else
//     display.sprite.setTextColor(textColor);

//   display.sprite.setTextDatum(TL_DATUM);
//   display.sprite.drawString(getName(), _x + 4, _y + 3);
// }

// void MenuItem::run()
// {
//   for (u8_t i = 0; i < functions.size(); i++)
//   {
//     if (ClickButtonUP.clicks == functions[i].clicksToRunUp || ClickButtonDOWN.clicks == functions[i].clicksToRunDown)
//     {
//       functions[i].func();
//       break;
//     }
//   }
// }

// // ###### MenuItemAction ######

// MenuItemAction::MenuItemAction(String _name, s8_t _clicksToRun, std::function<void()> _func) : MenuItem(_name)
// {
//   type = MenuItemType::Action;

//   addFunc(_clicksToRun, _func);
// }

// // ###### MenuItemNavigate ######

// MenuItemNavigate::MenuItemNavigate(String _name, String _target) : MenuItem(_name)
// {
//   type = MenuItemType::Navigate;
//   target = _target;

//   addFunc(2, [this]()
//           { screenManager.setScreen(target); });
// }

// void MenuItemNavigate::addRoute(s8_t _clicksToRun, String _target)
// {
//   addFunc(_clicksToRun, [this, _target]()
//           { screenManager.setScreen(_target); });
// }

// // ###### MenuItemBack ######

// MenuItemBack::MenuItemBack() : MenuItem("Back")
// {
//   type = MenuItemType::Back;

//   addFunc(2, []()
//           { screenManager.back(); });
// }

// // ###### MenuItemToggle ######

// MenuItemToggle::MenuItemToggle(String _name, bool *_value) : MenuItem(_name)
// {
//   type = MenuItemType::Toggle;
//   value = _value;

//   addFunc(2, [this]()
//           { *value = !*value; });
// }

// void MenuItemToggle::draw(u8_t _x, u8_t _y, bool _active)
// {
//   display.sprite.setTextSize(2 * TFT_SCALE);

//   if (_active)
//   {
//     // display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20 * TFT_SCALE, 5, bgColor);
// #ifdef TFT_BIG
//     display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 15, 37, 10, bgColor);
// #else
//     display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, bgColor);
// #endif
//     display.sprite.setTextColor(activeTextColor);
//   }
//   else
//     display.sprite.setTextColor(textColor);

//   display.sprite.setTextDatum(TL_DATUM);
//   display.sprite.drawString(getName(), _x + 4, _y + 3);

//   String valueStr = *value ? "ON" : "OFF";
//   display.sprite.setTextDatum(TR_DATUM);
//   display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 3);
// }

// // ###### MenuItemNumber ######

// MenuItemNumber::MenuItemNumber(String _name, long *_value, s16_t _min, s16_t _max) : MenuItem(_name)
// {
//   type = MenuItemType::Number;

//   value = _value;
//   min = _min;
//   max = _max;

//   isMutable = true;

//   addFunc(2, [this]()
//           { selected = !selected; });
// }

// MenuItemNumber::MenuItemNumber(String _name, long *_value) : MenuItem(_name)
// {
//   type = MenuItemType::Number;

//   value = _value;
//   min = 0;
//   max = 0;

//   isMutable = false;
// }

// bool MenuItemNumber::isSelected()
// {
//   return selected;
// }

// void MenuItemNumber::increase()
// {
//   if (isMutable && selected)
//     if (*value < max)
//       *value += 1;
// }

// void MenuItemNumber::decrease()
// {
//   if (isMutable && selected)
//     if (*value > min)
//       *value -= 1;
// }

// void MenuItemNumber::draw(u8_t _x, u8_t _y, bool _active)
// {

//   display.sprite.setTextSize(2 * TFT_SCALE);

//   if (!_active && selected)
//   {
//     selected = false;
//   }

//   String valueStr = String(*value);

//   if (_active && selected)
//   {
// // display.sprite.drawRoundRect(_x, _y, LCD_WIDTH - 10, 20 * TFT_SCALE, 5, bgColor);
// #ifdef TFT_BIG
//     display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 15, 37, 10, bgColor);
// #else
//     display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, bgColor);
// #endif
//     display.sprite.setTextColor(textColor);
//     display.sprite.setTextDatum(TL_DATUM);
//     display.sprite.drawString(getName(), _x + 4, _y + 2);
//     display.sprite.setTextDatum(TR_DATUM);
//     display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 3);
//   }
//   else if (_active)
//   {
//     // display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20 * TFT_SCALE, 5, bgColor);
// #ifdef TFT_BIG
//     display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 15, 37, 10, bgColor);
// #else
//     display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, bgColor);
// #endif
//     display.sprite.setTextColor(activeTextColor);
//     display.sprite.drawString(getName(), _x + 4, _y + 2);
//     display.sprite.setTextDatum(TR_DATUM);
//     display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 3);
//   }
//   else
//   {
//     display.sprite.setTextColor(textColor);
//     display.sprite.drawString(getName(), _x + 4, _y + 2);
//     display.sprite.setTextDatum(TR_DATUM);
//     display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 3);
//   }
// }

// // ###### Menu ######

// Menu::Menu()
// {
//   active = 0;
//   offsetFromTop = 0;
// }

// void Menu::setItemsPerPage(u8_t _itemsPerPage)
// {
//   maxItemsPerPage = _itemsPerPage;
// }

// u8_t Menu::getItemsPerPage()
// {
//   return maxItemsPerPage;
// }

// void Menu::setActive(u8_t _active)
// {
//   active = _active;
// }

// u8_t Menu::getActive()
// {
//   return active;
// }

// void Menu::nextItem()
// {
//   if (active < numItems - 1)
//     active++;
// }

// void Menu::prevItem()
// {
//   if (active > 0)
//     active--;
// }

// void Menu::addMenuItem(MenuItem *_item)
// {
//   items.push_back(_item);

//   numItems = items.size();
//   numItemsPerPage = numItems < maxItemsPerPage ? numItems : maxItemsPerPage;
// }

// void Menu::draw()
// {
//   // display.sprite.setTextSize(1);
//   // display.sprite.setTextColor(TFT_WHITE);
//   display.sprite.setTextDatum(TL_DATUM);

//   // char buf[50];
//   // sprintf(buf, "Menu (%d/%d)", active, offsetFromTop);

//   // display.sprite.drawString(buf, 0, 0);

//   display.sprite.setTextSize(2 * TFT_SCALE);

//   if (active - offsetFromTop >= numItemsPerPage - 1 && active < numItems - 1)
//     offsetFromTop = active - numItemsPerPage + 2;
//   if (active <= offsetFromTop && active > 0)
//     offsetFromTop = active - 1;

//   for (u8_t i = 0; i < numItemsPerPage; i++)
//   {
//     MenuItem *item = items[i + offsetFromTop];

// #ifdef TFT_BIG
//     item->draw(5, 45 + (i * 39), active == i + offsetFromTop);
// #else
//     item->draw(5, 25 + (i * 20), active == i + offsetFromTop);
// #endif
//   }

// #ifdef TFT_BIG
//   u8_t scrollBarPosition = (LCD_HEIGHT - 46) / (numItems < 1 ? 1 : numItems) * active;
//   u8_t scrollBarHeight = (numItems < 1 ? 1 : numItems) - 1 == active ? LCD_HEIGHT - 45 - scrollBarPosition : (LCD_HEIGHT - 46) / (numItems < 1 ? 1 : numItems);

//   display.sprite.drawLine(LCD_WIDTH - 3, 45, LCD_WIDTH - 3, LCD_HEIGHT - 1, TFT_WHITE);
//   display.sprite.fillRect(LCD_WIDTH - 5, 45 + scrollBarPosition, 5, scrollBarHeight, TFT_WHITE);
// #else
//   u8_t scrollBarPosition = (LCD_HEIGHT - 31) / (numItems < 1 ? 1 : numItems) * active;
//   u8_t scrollBarHeight = (numItems < 1 ? 1 : numItems) - 1 == active ? LCD_HEIGHT - 30 - scrollBarPosition : (LCD_HEIGHT - 31) / (numItems < 1 ? 1 : numItems);

//   display.sprite.drawLine(LCD_WIDTH - 2, 30, LCD_WIDTH - 2, LCD_HEIGHT - 1, TFT_WHITE);
//   display.sprite.fillRect(LCD_WIDTH - 3, 30 + scrollBarPosition, 3, scrollBarHeight, TFT_WHITE);
// #endif
// }

// void Menu::update()
// {
//   if (numItems < 1)
//   {
//     if (ClickButtonUP.clicks == 1 || ClickButtonDOWN.clicks == 1)
//       screenManager.back();

//     return;
//   }

//   if (ClickButtonUP.clicks != 0 && ClickButtonUP.clicks != 1 || ClickButtonDOWN.clicks != 0 && ClickButtonDOWN.clicks != 1)
//     items[active]->run();

//   if (ClickButtonDOWN.clicks == 1)
//   {
//     if (items[active]->getType() == MenuItemType::Number && ((MenuItemNumber *)items[active])->isSelected())
//       ((MenuItemNumber *)items[active])->decrease();
//     else
//       nextItem();
//     ESP_LOGI(TAG, "Down. Active: %d. clicks: %d. pressed: %d", active, ClickButtonDOWN.clicks, ClickButtonDOWN.depressed);
//   }
//   else if (ClickButtonUP.clicks == 1)
//   {
//     if (items[active]->getType() == MenuItemType::Number && ((MenuItemNumber *)items[active])->isSelected())
//       ((MenuItemNumber *)items[active])->increase();
//     else
//       prevItem();
//     ESP_LOGI(TAG, "Up. Active: %d. clicks: %d. pressed: %d", active, ClickButtonUP.clicks, ClickButtonUP.depressed);
//   }
// }