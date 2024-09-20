#include "Menu.h"

static const char *TAG = "Menu";

// ###### MenuItem ######

void MenuItem::draw(uint8_t _x, uint8_t _y, bool _active)
{
  display.sprite.setTextSize(2 * TFT_SCALE);

  if (_active)
  {
#ifdef TFT_BIG
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 15, 37, 10, bgColor);
#else
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, bgColor);
#endif

    display.sprite.setTextColor(activeTextColor);
  }
  else
    display.sprite.setTextColor(textColor);

  display.sprite.setTextDatum(TL_DATUM);
  display.sprite.drawString(getName(), _x + 4, _y + 3);
}

// ###### MenuItemToggle ######

void MenuItemToggle::draw(uint8_t _x, uint8_t _y, bool _active)
{
  display.sprite.setTextSize(2 * TFT_SCALE);

  if (_active)
  {
    // display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20 * TFT_SCALE, 5, bgColor);
#ifdef TFT_BIG
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 15, 37, 10, bgColor);
#else
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, bgColor);
#endif
    display.sprite.setTextColor(activeTextColor);
  }
  else
    display.sprite.setTextColor(textColor);

  display.sprite.setTextDatum(TL_DATUM);
  display.sprite.drawString(getName(), _x + 4, _y + 3);

  String valueStr = *value ? "ON" : "OFF";
  display.sprite.setTextDatum(TR_DATUM);
  display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 3);
}

// ###### MenuItemNumber ######

template <typename T>
void MenuItemNumber<T>::draw(uint8_t _x, uint8_t _y, bool _active)
{

  display.sprite.setTextSize(2 * TFT_SCALE);

  if (!_active && selected)
  {
    selected = false;
  }

  String valueStr = String(*value);

  if (_active && selected)
  {
// display.sprite.drawRoundRect(_x, _y, LCD_WIDTH - 10, 20 * TFT_SCALE, 5, bgColor);
#ifdef TFT_BIG
    display.sprite.drawRoundRect(_x, _y, LCD_WIDTH - 15, 37, 10, bgColor);
#else
    display.sprite.drawRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, bgColor);
#endif
    display.sprite.setTextColor(textColor);
    display.sprite.setTextDatum(TL_DATUM);
    display.sprite.drawString(getName(), _x + 4, _y + 2);
    display.sprite.setTextDatum(TR_DATUM);
    display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 3);
  }
  else if (_active)
  {
    // display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20 * TFT_SCALE, 5, bgColor);
#ifdef TFT_BIG
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 15, 37, 10, bgColor);
#else
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 10, 20, 5, bgColor);
#endif
    display.sprite.setTextColor(activeTextColor);
    display.sprite.setTextDatum(TL_DATUM);
    display.sprite.drawString(getName(), _x + 4, _y + 2);
    display.sprite.setTextDatum(TR_DATUM);
    display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 3);
  }
  else
  {
    display.sprite.setTextColor(textColor);
    display.sprite.setTextDatum(TL_DATUM);
    display.sprite.drawString(getName(), _x + 4, _y + 2);
    display.sprite.setTextDatum(TR_DATUM);
    display.sprite.drawString(valueStr, LCD_WIDTH - 10, _y + 3);
  }
}

// ###### Menu ######

void Menu::draw()
{
  display.sprite.setTextDatum(TL_DATUM);

  //   uint8_t itemMap[numItems];
  //   uint8_t numItemsVisible = 0;
  //   for (uint8_t i = 0; i < numItems; i++)
  //   {
  //     if (!items[i]->isHidden())
  //     {
  //       itemMap[numItemsVisible] = i;
  //       numItemsVisible++;
  //     }
  //   }

  //   for (uint8_t i = 0; i < numItemsPerPage; i++)
  //   {
  //     uint8_t topItem;

  //     if (active <= 0) // if the top item is selected
  //       topItem = 0;
  //     else if (active >= static_cast<u8_t>(numItemsVisible - 1)) // if the bottom item is selected
  //       if (numItemsVisible < numItemsPerPage)                                 // if there are less than 3 items
  //         topItem = 0;
  //       else // if there are more than 3 items
  //         topItem = static_cast<u8_t>(numItemsVisible - numItemsPerPage);
  //     else // if any other item is selected
  //       topItem = active - 1;

  //     uint8_t itemIdx = i + topItem;

  //     MenuItem *item = items[itemMap[itemIdx]];

  // #ifdef TFT_BIG
  //     item->draw(5, 45 + (i * 39), active == itemMap[itemIdx]);
  // #else
  //     item->draw(5, 25 + (i * 20), active == itemMap[itemIdx]);
  // #endif
  //   }

  if (active - offsetFromTop >= numItemsPerPage - 1 && active < numItems - 1)
    offsetFromTop = active - numItemsPerPage + 2;
  if (active <= offsetFromTop && active > 0)
    offsetFromTop = active - 1;

  for (uint8_t i = 0; i < numItemsPerPage; i++)
  {
    MenuItem *item = items[i + offsetFromTop];

#ifdef TFT_BIG
    item->draw(5, 45 + (i * 39), active == i + offsetFromTop);
#else
    item->draw(5, 25 + (i * 20), active == i + offsetFromTop);
#endif
  }

#ifdef TFT_BIG
  uint8_t scrollBarPosition = (LCD_HEIGHT - 46) / (numItems < 1 ? 1 : numItems) * active;
  uint8_t scrollBarHeight = (numItems < 1 ? 1 : numItems) - 1 == active ? LCD_HEIGHT - 45 - scrollBarPosition : (LCD_HEIGHT - 46) / (numItems < 1 ? 1 : numItems);

  display.sprite.drawLine(LCD_WIDTH - 3, 45, LCD_WIDTH - 3, LCD_HEIGHT - 1, TFT_WHITE);
  display.sprite.fillRect(LCD_WIDTH - 5, 45 + scrollBarPosition, 5, scrollBarHeight, TFT_WHITE);
#else
  uint8_t scrollBarPosition = (LCD_HEIGHT - 31) / (numItems < 1 ? 1 : numItems) * active;
  uint8_t scrollBarHeight = (numItems < 1 ? 1 : numItems) - 1 == active ? LCD_HEIGHT - 30 - scrollBarPosition : (LCD_HEIGHT - 31) / (numItems < 1 ? 1 : numItems);

  display.sprite.drawLine(LCD_WIDTH - 2, 30, LCD_WIDTH - 2, LCD_HEIGHT - 1, TFT_WHITE);
  display.sprite.fillRect(LCD_WIDTH - 3, 30 + scrollBarPosition, 3, scrollBarHeight, TFT_WHITE);
#endif
}

void MenuItem::run()
{
  for (uint8_t i = 0; i < functions.size(); i++)
  {
    if (ClickButtonUP.clicks == functions[i].clicksToRunUp || ClickButtonDOWN.clicks == functions[i].clicksToRunDown)
    {
      functions[i].func();
      break;
    }
  }
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

    if ((items[active]->getType() == MenuItemType::Navigate || items[active]->getType() == MenuItemType::Back) && ClickButtonTRIGGER.clicks == 1)
    {
      items[active]->executeFunc();
    }

  if (ClickButtonDOWN.clicks == 1)
  {
    if (items[active]->getType() == MenuItemType::Number)
    {
      if (((MenuItemNumber<int> *)items[active])->isSelected())
        ((MenuItemNumber<int> *)items[active])->increase();
      else if (((MenuItemNumber<long> *)items[active])->isSelected())
        ((MenuItemNumber<long> *)items[active])->increase();
      else if (((MenuItemNumber<uint8_t> *)items[active])->isSelected())
        ((MenuItemNumber<uint8_t> *)items[active])->increase();
      else
        nextItem();
    }
    else
      nextItem();
    ESP_LOGI(TAG, "Down. Active: %d. clicks: %d. pressed: %d", active, ClickButtonDOWN.clicks, ClickButtonDOWN.depressed);
  }
  else if (ClickButtonUP.clicks == 1)
  {
    if (items[active]->getType() == MenuItemType::Number)
    {
      if (((MenuItemNumber<int> *)items[active])->isSelected())
        ((MenuItemNumber<int> *)items[active])->decrease();
      else if (((MenuItemNumber<long> *)items[active])->isSelected())
        ((MenuItemNumber<long> *)items[active])->decrease();
      else if (((MenuItemNumber<uint8_t> *)items[active])->isSelected())
        ((MenuItemNumber<uint8_t> *)items[active])->decrease();
      else
        prevItem();
    }
    else
      prevItem();
    ESP_LOGI(TAG, "Up. Active: %d. clicks: %d. pressed: %d", active, ClickButtonUP.clicks, ClickButtonUP.depressed);
  }
}