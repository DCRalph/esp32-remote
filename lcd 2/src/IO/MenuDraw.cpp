#include "Menu.h"
#include "Buttons.h"

static const char *TAG = "Menu";

namespace
{
  constexpr uint16_t kMenuBg = TFT_BLACK;
  constexpr uint16_t kMenuText = TFT_WHITE;
  constexpr uint16_t kMenuActiveBg = TFT_WHITE;
  constexpr uint16_t kMenuActiveText = TFT_BLACK;

  struct MenuDrawStyle
  {
    uint8_t textSize;
    int lineHeight;
    uint8_t radius;
  };

  MenuDrawStyle getStyle(MenuSize size)
  {
    switch (size)
    {
    case MenuSize::Small:
      return {1, 12, 2};
    case MenuSize::Medium:
      return {2, 20, 3};
    case MenuSize::Large:
    default:
      return {3, 28, 4};
    }
  }

  int calcTextY(int y, const MenuDrawStyle &style)
  {
    return y + (style.lineHeight - display.sprite.fontHeight()) / 2;
  }

  int readSelectClicks()
  {
    // Treat any non-navigation click (long/double) as a select click.
    if (ClickButtonDOWN.clicks != 0 && ClickButtonDOWN.clicks != 1)
      return 1;
    if (ClickButtonUP.clicks != 0 && ClickButtonUP.clicks != 1)
      return 1;
    return 0;
  }

  int readEncoderDelta()
  {
    if (ClickButtonDOWN.clicks == 1)
      return 1;
    if (ClickButtonUP.clicks == 1)
      return -1;
    return 0;
  }
} // namespace

// ###### MenuItem ######

void MenuItem::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuDrawStyle style = getStyle(size);

  display.sprite.setTextSize(style.textSize);
  display.sprite.setTextDatum(TL_DATUM);

  if (_active)
  {
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 4, style.lineHeight, style.radius, kMenuActiveBg);
    display.sprite.setTextColor(kMenuActiveText, kMenuActiveBg);
  }
  else
  {
    display.sprite.setTextColor(kMenuText, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.sprite.drawString(getName(), _x + 2, textY);
}

// ###### MenuItemToggle ######

void MenuItemToggle::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuDrawStyle style = getStyle(size);

  display.sprite.setTextSize(style.textSize);
  display.sprite.setTextDatum(TL_DATUM);

  if (_active)
  {
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 4, style.lineHeight, style.radius, kMenuActiveBg);
    display.sprite.setTextColor(kMenuActiveText, kMenuActiveBg);
  }
  else
  {
    display.sprite.setTextColor(kMenuText, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.sprite.drawString(getName(), _x + 2, textY);

  String valueStr = *value ? "ON" : "OFF";
  display.sprite.setTextDatum(TR_DATUM);
  display.sprite.drawString(valueStr, LCD_WIDTH - 6, textY);
}

// ###### MenuItemString ######

void MenuItemString::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuDrawStyle style = getStyle(size);

  display.sprite.setTextSize(style.textSize);
  display.sprite.setTextDatum(TL_DATUM);

  if (_active)
  {
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 4, style.lineHeight, style.radius, kMenuActiveBg);
    display.sprite.setTextColor(kMenuActiveText, kMenuActiveBg);
  }
  else
  {
    display.sprite.setTextColor(kMenuText, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.sprite.drawString(getName(), _x + 2, textY);
  display.sprite.setTextDatum(TR_DATUM);
  display.sprite.drawString(getValue(), LCD_WIDTH - 6, textY);
}

// ###### MenuItemNumber ######

template <typename T>
void MenuItemNumber<T>::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuDrawStyle style = getStyle(size);

  if (!_active && selected)
  {
    selected = false;
  }

  display.sprite.setTextSize(style.textSize);
  display.sprite.setTextDatum(TL_DATUM);

  String valueStr = String(*value);

  if (_active && selected)
  {
    display.sprite.drawRoundRect(_x, _y, LCD_WIDTH - 4, style.lineHeight, style.radius, kMenuText);
    display.sprite.setTextColor(kMenuText, kMenuBg);
  }
  else if (_active)
  {
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 4, style.lineHeight, style.radius, kMenuActiveBg);
    display.sprite.setTextColor(kMenuActiveText, kMenuActiveBg);
  }
  else
  {
    display.sprite.setTextColor(kMenuText, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.sprite.drawString(getName(), _x + 2, textY);

  display.sprite.setTextDatum(TR_DATUM);
  display.sprite.drawString(valueStr, LCD_WIDTH - 6, textY);
}

// ###### MenuItemSelect ######

void MenuItemSelect::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuDrawStyle style = getStyle(size);
  String dispText = getName();
  String optionText = getSelectedOption();

  display.sprite.setTextSize(style.textSize);
  display.sprite.setTextDatum(TL_DATUM);

  if (_active && selected)
  {
    display.sprite.drawRoundRect(_x, _y, LCD_WIDTH - 4, style.lineHeight, style.radius, kMenuText);
    display.sprite.setTextColor(kMenuText, kMenuBg);
  }
  else if (_active)
  {
    display.sprite.fillRoundRect(_x, _y, LCD_WIDTH - 4, style.lineHeight, style.radius, kMenuActiveBg);
    display.sprite.setTextColor(kMenuActiveText, kMenuActiveBg);
  }
  else
  {
    display.sprite.setTextColor(kMenuText, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.sprite.drawString(dispText, _x + 2, textY);
  display.sprite.setTextDatum(TR_DATUM);
  display.sprite.drawString(optionText, LCD_WIDTH - 6, textY);
}

// ###### Menu ######

void Menu::draw()
{
  // Delegate to active submenu if present
  if (activeSubmenu)
  {
    activeSubmenu->draw();
    return;
  }

  MenuDrawStyle style = getStyle(menuSize);
  int lineHeight = style.lineHeight;
  int startY = 22;

  uint8_t itemMap[numItems];

  uint8_t numItemsVisible = 0;
  uint8_t numItemsHidden = 0;
  for (uint8_t i = 0; i < numItems; i++)
  {
    if (!items[i]->isHidden())
    {
      itemMap[numItemsVisible] = i;
      numItemsVisible++;
    }

    if (items[i]->isHidden() && active > i)
      numItemsHidden++;
  }

  uint8_t calcActive = active - numItemsHidden;

  // Check if we have any visible items to draw
  if (numItemsVisible == 0)
  {
    return;
  }

  uint8_t itemRelIdx = calcActive - topItem;

  // if first item on screen is selected and item is the first item in the list - update topItem
  if (itemRelIdx <= 0 && calcActive == 0)
    topItem = 0;

  // if first item on screen is selected and item is not the first item in the list - update topItem
  if (itemRelIdx <= 0 && calcActive > 0)
    topItem--;

  // if last item on screen is selected and item is not the last item in the list - update topItem
  if (itemRelIdx >= numItemsPerPage - 1 && calcActive < numItemsVisible - 1)
    topItem++;

  // if last item on screen is selected and item is the last item in the list - update topItem
  if (itemRelIdx >= numItemsPerPage - 1 && calcActive == numItemsVisible - 1)
    topItem = numItemsVisible - numItemsPerPage;

  for (uint8_t i = 0; i < numItemsPerPage; i++)
  {
    uint8_t itemIdx = i + topItem;

    // Add bounds checking to prevent crash
    if (itemIdx >= numItemsVisible)
    {
      break;
    }

    MenuItem *item = items[itemMap[itemIdx]];

    item->draw(0, startY + (i * lineHeight), active == itemMap[itemIdx]);
  }

  int trackStart = startY;
  int trackHeight = LCD_HEIGHT - trackStart - 1;
  int visibleCount = numItemsVisible < 1 ? 1 : numItemsVisible;
  int scrollBarPosition = (trackHeight - 1) / visibleCount * calcActive;
  int scrollBarHeight = (visibleCount - 1 == calcActive) ? trackHeight - scrollBarPosition : (trackHeight - 1) / visibleCount;

  display.sprite.drawLine(LCD_WIDTH - 2, trackStart, LCD_WIDTH - 2, LCD_HEIGHT - 1, kMenuText);
  display.sprite.fillRect(LCD_WIDTH - 3, trackStart + scrollBarPosition, 3, scrollBarHeight, kMenuText);
}

void MenuItem::run()
{
  ESP_LOGI(TAG, "Running %s", name.c_str());
  ESP_LOGI(TAG, "Functions: %d", functions.size());

  int selectClicks = readSelectClicks();

  for (uint8_t i = 0; i < functions.size(); i++)
  {
    if (selectClicks == functions[i].clicksToRun)
    {
      functions[i].func();
      break;
    }
  }
}

void Menu::update()
{
  // Delegate to active submenu if present
  if (activeSubmenu)
  {
    activeSubmenu->update();
    return;
  }

  int selectClicks = readSelectClicks();

  if (numItems < 1)
  {
    if (selectClicks == 1)
    {
      screenManager.back();
    }

    return;
  }

  if (selectClicks != 0)
  {
    ESP_LOGI(TAG, "Clicks: %d", selectClicks);
    items[active]->run();
  }

  MenuItem *currentItem = items[active];
  int encoderDelta = readEncoderDelta();

  if (encoderDelta > 0)
  {

    switch (currentItem->getType())
    {
    case MenuItemType::Select:
    {
      MenuItemSelect *selectItem = (MenuItemSelect *)currentItem;

      if (selectItem && selectItem->isSelected())
        selectItem->prevOption();
      else
        nextItem();
    }
    break;

    case MenuItemType::Number:
    {
      MenuItemNumberBase *numberItem = (MenuItemNumberBase *)currentItem;

      if (numberItem && numberItem->isSelected())
        numberItem->decrease();
      else
        nextItem();
    }
    break;

    default:
      nextItem();
      break;
    }
  }
  else if (encoderDelta < 0)
  {

    switch (currentItem->getType())
    {
    case MenuItemType::Select:
    {
      MenuItemSelect *selectItem = (MenuItemSelect *)currentItem;

      if (selectItem && selectItem->isSelected())
        selectItem->nextOption();
      else
        prevItem();
    }
    break;

    case MenuItemType::Number:
    {
      MenuItemNumberBase *numberItem = (MenuItemNumberBase *)currentItem;

      if (numberItem && numberItem->isSelected())
        numberItem->increase();
      else
        prevItem();
    }
    break;

    default:
      prevItem();
      break;
    }
  }
}