#include "Menu.h"
#include "Haptic.h"

static const char *TAG = "Menu";

// ###### MenuItem ######

void MenuItem::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;

  if (size == MenuSize::Small)
  {
    display.u8g2.setFont(u8g2_font_6x10_tf);

    if (_active)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 9);
      display.u8g2.setDrawColor(0);
    }
    else
      display.u8g2.setDrawColor(1);
    display.u8g2.drawStr(_x + 1, _y + 8, getName().c_str());
  }
  else if (size == MenuSize::Medium)
  {
    display.u8g2.setFont(u8g2_font_doomalpha04_tr);

    if (_active)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 12);
      display.u8g2.setDrawColor(0);
    }
    else
      display.u8g2.setDrawColor(1);
    display.u8g2.drawStr(_x + 1, _y + 11, getName().c_str());
  }
  else
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
}

// ###### MenuItemToggle ######

void MenuItemToggle::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;

  if (size == MenuSize::Small)
  {
    display.u8g2.setFont(u8g2_font_6x10_tf);

    if (_active)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 9);
      display.u8g2.setDrawColor(0);
    }
    else
      display.u8g2.setDrawColor(1);
    display.u8g2.drawStr(_x + 1, _y + 8, getName().c_str());

    String valueStr = *value ? "ON" : "OFF";
    display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 8, valueStr.c_str());
  }
  else if (size == MenuSize::Medium)
  {
    display.u8g2.setFont(u8g2_font_doomalpha04_tr);

    if (_active)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 12);
      display.u8g2.setDrawColor(0);
    }
    else
      display.u8g2.setDrawColor(1);
    display.u8g2.drawStr(_x + 1, _y + 11, getName().c_str());

    String valueStr = *value ? "ON" : "OFF";
    display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 11, valueStr.c_str());
  }
  else
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
}

// ###### MenuItemString ######

void MenuItemString::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;

  if (size == MenuSize::Small)
  {
    display.u8g2.setFont(u8g2_font_6x10_tf);

    if (_active)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 9);
      display.u8g2.setDrawColor(0);
    }
    else
      display.u8g2.setDrawColor(1);

    display.u8g2.drawStr(_x + 1, _y + 8, getName().c_str());
    display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(getValue().c_str()) - 5, _y + 8, getValue().c_str());
  }
  else if (size == MenuSize::Medium)
  {
    display.u8g2.setFont(u8g2_font_doomalpha04_tr);

    if (_active)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 12);
      display.u8g2.setDrawColor(0);
    }
    else
      display.u8g2.setDrawColor(1);

    display.u8g2.drawStr(_x + 1, _y + 11, getName().c_str());
    display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(getValue().c_str()) - 5, _y + 11, getValue().c_str());
  }
  else
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
    display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(getValue().c_str()) - 5, _y + 15, getValue().c_str());
  }
}

// ###### MenuItemNumber ######

template <typename T>
void MenuItemNumber<T>::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;

  if (size == MenuSize::Small)
  {
    display.u8g2.setFont(u8g2_font_6x10_tf);

    if (!_active && selected)
    {
      selected = false;
    }

    String valueStr = String(*value);

    if (_active && selected)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawFrame(_x, _y, DISPLAY_WIDTH - 4, 9);
      display.u8g2.drawStr(_x + 1, _y + 8, getName().c_str());
      display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 8, valueStr.c_str());
    }
    else if (_active)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 9);
      display.u8g2.setDrawColor(0);
      display.u8g2.drawStr(_x + 1, _y + 8, getName().c_str());
      display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 8, valueStr.c_str());
    }
    else
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawStr(_x + 1, _y + 8, getName().c_str());
      display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 8, valueStr.c_str());
    }
  }
  else if (size == MenuSize::Medium)
  {
    display.u8g2.setFont(u8g2_font_doomalpha04_tr);

    if (!_active && selected)
    {
      selected = false;
    }

    String valueStr = String(*value);

    if (_active && selected)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawFrame(_x, _y, DISPLAY_WIDTH - 4, 12);
      display.u8g2.drawStr(_x + 1, _y + 11, getName().c_str());
      display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 11, valueStr.c_str());
    }
    else if (_active)
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 12);
      display.u8g2.setDrawColor(0);
      display.u8g2.drawStr(_x + 1, _y + 11, getName().c_str());
      display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 11, valueStr.c_str());
    }
    else
    {
      display.u8g2.setDrawColor(1);
      display.u8g2.drawStr(_x + 1, _y + 11, getName().c_str());
      display.u8g2.drawStr(DISPLAY_WIDTH - display.u8g2.getStrWidth(valueStr.c_str()) - 5, _y + 11, valueStr.c_str());
    }
  }
  else
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
}

// ###### MenuItemSelect ######

void MenuItemSelect::draw(uint8_t _x, uint8_t _y, bool _active)
{
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  String dispText = getName();
  String optionText = getSelectedOption();

  if (size == MenuSize::Small)
  {
    display.u8g2.setFont(u8g2_font_6x10_tf);

    if (_active && selected)
    {
      // Editing mode: draw a frame around the option.
      display.u8g2.setDrawColor(1);
      display.u8g2.drawFrame(_x, _y, DISPLAY_WIDTH - 4, 9);
      display.u8g2.drawStr(_x + 1, _y + 8, dispText.c_str());
      display.u8g2.drawStr(
          DISPLAY_WIDTH - display.u8g2.getStrWidth(optionText.c_str()) - 5,
          _y + 8,
          optionText.c_str());
    }
    else if (_active)
    {
      // Active but not editing: draw with a filled background.
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 9);
      display.u8g2.setDrawColor(0);
      display.u8g2.drawStr(_x + 1, _y + 8, dispText.c_str());
      display.u8g2.drawStr(
          DISPLAY_WIDTH - display.u8g2.getStrWidth(optionText.c_str()) - 5,
          _y + 8,
          optionText.c_str());
    }
    else
    {
      // Inactive: normal drawing.
      display.u8g2.setDrawColor(1);
      display.u8g2.drawStr(_x + 1, _y + 8, dispText.c_str());
      display.u8g2.drawStr(
          DISPLAY_WIDTH - display.u8g2.getStrWidth(optionText.c_str()) - 5,
          _y + 8,
          optionText.c_str());
    }
  }
  else if (size == MenuSize::Medium)
  {
    display.u8g2.setFont(u8g2_font_doomalpha04_tr);

    if (_active && selected)
    {
      // Editing mode: draw a frame around the option.
      display.u8g2.setDrawColor(1);
      display.u8g2.drawFrame(_x, _y, DISPLAY_WIDTH - 4, 12);
      display.u8g2.drawStr(_x + 1, _y + 11, dispText.c_str());
      display.u8g2.drawStr(
          DISPLAY_WIDTH - display.u8g2.getStrWidth(optionText.c_str()) - 5,
          _y + 11,
          optionText.c_str());
    }
    else if (_active)
    {
      // Active but not editing: draw with a filled background.
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 12);
      display.u8g2.setDrawColor(0);
      display.u8g2.drawStr(_x + 1, _y + 11, dispText.c_str());
      display.u8g2.drawStr(
          DISPLAY_WIDTH - display.u8g2.getStrWidth(optionText.c_str()) - 5,
          _y + 11,
          optionText.c_str());
    }
    else
    {
      // Inactive: normal drawing.
      display.u8g2.setDrawColor(1);
      display.u8g2.drawStr(_x + 1, _y + 11, dispText.c_str());
      display.u8g2.drawStr(
          DISPLAY_WIDTH - display.u8g2.getStrWidth(optionText.c_str()) - 5,
          _y + 11,
          optionText.c_str());
    }
  }
  else
  {
    // Draw similar to other items.
    display.u8g2.setFont(u8g2_font_profont22_tf);

    if (_active && selected)
    {
      // Editing mode: draw a frame around the option.
      display.u8g2.setDrawColor(1);
      display.u8g2.drawFrame(_x, _y, DISPLAY_WIDTH - 4, 16);
      display.u8g2.drawStr(_x + 1, _y + 15, dispText.c_str());
      display.u8g2.drawStr(
          DISPLAY_WIDTH - display.u8g2.getStrWidth(optionText.c_str()) - 5,
          _y + 15,
          optionText.c_str());
    }
    else if (_active)
    {
      // Active but not editing: draw with a filled background.
      display.u8g2.setDrawColor(1);
      display.u8g2.drawBox(_x, _y, DISPLAY_WIDTH - 4, 16);
      display.u8g2.setDrawColor(0);
      display.u8g2.drawStr(_x + 1, _y + 15, dispText.c_str());
      display.u8g2.drawStr(
          DISPLAY_WIDTH - display.u8g2.getStrWidth(optionText.c_str()) - 5,
          _y + 15,
          optionText.c_str());
    }
    else
    {
      // Inactive: normal drawing.
      display.u8g2.setDrawColor(1);
      display.u8g2.drawStr(_x + 1, _y + 15, dispText.c_str());
      display.u8g2.drawStr(
          DISPLAY_WIDTH - display.u8g2.getStrWidth(optionText.c_str()) - 5,
          _y + 15,
          optionText.c_str());
    }
  }
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

  int lineHeight;
  int startY = 12;

  if (menuSize == MenuSize::Small)
  {
    lineHeight = 8;
    display.u8g2.setFont(u8g2_font_6x10_tf);
  }
  else if (menuSize == MenuSize::Medium)
  {
    lineHeight = 12;
    display.u8g2.setFont(u8g2_font_doomalpha04_tr);
  }
  else
  {
    lineHeight = 18;
    display.u8g2.setFont(u8g2_font_profont22_tf);
  }

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

  display.u8g2.setDrawColor(1);

  uint8_t scrollBarPosition = (DISPLAY_HEIGHT - 13) / (numItemsVisible < 1 ? 1 : numItemsVisible) * calcActive;
  uint8_t scrollBarHeight = (numItemsVisible < 1 ? 1 : numItemsVisible) - 1 == calcActive ? DISPLAY_HEIGHT - 12 - scrollBarPosition : (DISPLAY_HEIGHT - 13) / (numItemsVisible < 1 ? 1 : numItemsVisible);

  display.u8g2.drawLine(DISPLAY_WIDTH - 2, 12, DISPLAY_WIDTH - 2, DISPLAY_HEIGHT - 1);
  display.u8g2.drawBox(DISPLAY_WIDTH - 3, 12 + scrollBarPosition, 3, scrollBarHeight);
}

void MenuItem::run()
{

  ESP_LOGI(TAG, "Running %s", name.c_str());
  ESP_LOGI(TAG, "Functions: %d", functions.size());

  for (uint8_t i = 0; i < functions.size(); i++)
  {
    if (ClickButtonEnc.clicks == functions[i].clicksToRun)
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

  if (numItems < 1)
  {
    if (ClickButtonEnc.clicks == 1)
    {
      screenManager.back();
      haptic.playEffect(40);
    }

    return;
  }

  if (ClickButtonEnc.clicks != 0)
  {
    ESP_LOGI(TAG, "Clicks: %d", ClickButtonEnc.clicks);
    items[active]->run();
    haptic.playEffect(40);
  }

  MenuItem *currentItem = items[active];

  if (encoderGetCount() > 0)
  {

    haptic.playEffect(4);

    switch (currentItem->getType())
    {
    case MenuItemType::Select:
    {
      MenuItemSelect *selectItem = (MenuItemSelect *)currentItem;

      if (selectItem && selectItem->isSelected())
        selectItem->nextOption();
      else
        nextItem();
    }
    break;

    case MenuItemType::Number:
    {
      MenuItemNumberBase *numberItem = (MenuItemNumberBase *)currentItem;

      if (numberItem && numberItem->isSelected())
        numberItem->increase();
      else
        nextItem();
    }
    break;

    default:
      nextItem();
      break;
    }

    // nextItem();

    encoderClearCount();
  }
  else if (encoderGetCount() < 0)
  {

    haptic.playEffect(4);

    switch (currentItem->getType())
    {
    case MenuItemType::Select:
    {
      MenuItemSelect *selectItem = (MenuItemSelect *)currentItem;

      if (selectItem && selectItem->isSelected())
        selectItem->prevOption();
      else
        prevItem();
    }
    break;

    case MenuItemType::Number:
    {
      MenuItemNumberBase *numberItem = (MenuItemNumberBase *)currentItem;

      if (numberItem && numberItem->isSelected())
        numberItem->decrease();
      else
        prevItem();
    }
    break;

    default:
      prevItem();
      break;
    }

    // prevItem();

    encoderClearCount();
  }
}