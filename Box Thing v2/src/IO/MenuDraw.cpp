#include "Menu.h"

static const char *TAG = "Menu";

// ###### MenuItem ######

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

// ###### MenuItemToggle ######

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

template <typename T>
void MenuItemNumber<T>::draw(uint8_t _x, uint8_t _y, bool _active)
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

// ###### MenuItemSelect ######

void MenuItemSelect::draw(uint8_t _x, uint8_t _y, bool _active)
{
  // Draw similar to other items.
  display.u8g2.setFont(u8g2_font_profont22_tf);

  String dispText = getName();
  String optionText = getSelectedOption();

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

// ###### Menu ######

void Menu::draw()
{
  display.u8g2.setFont(u8g2_font_profont22_tf);

  uint8_t itemMap[numItems];

  uint8_t numItemsVisible = 0;
  for (uint8_t i = 0; i < numItems; i++)
  {
    if (!items[i]->isHidden())
    {
      itemMap[numItemsVisible] = i;
      numItemsVisible++;
    }
  }

  for (uint8_t i = 0; i < numItemsPerPage; i++)
  {
    uint8_t topItem;

    if (active <= 0) // if the top item is selected
      topItem = 0;
    else if (active >= static_cast<u8_t>(numItemsVisible - 1)) // if the bottom item is selected
      if (numItemsVisible < 3)                                 // if there are less than 3 items
        topItem = 0;
      else // if there are more than 3 items
        topItem = static_cast<u8_t>(numItemsVisible - 3);
    else // if any other item is selected
      topItem = active - 1;

    uint8_t itemIdx = i + topItem;

    MenuItem *item = items[itemMap[itemIdx]];

    item->draw(0, 12 + (i * 18), active == itemMap[itemIdx]);
  }

  display.u8g2.setDrawColor(1);

  uint8_t scrollBarPosition = (DISPLAY_HEIGHT - 13) / (numItems < 1 ? 1 : numItems) * active;
  uint8_t scrollBarHeight = (numItems < 1 ? 1 : numItems) - 1 == active ? DISPLAY_HEIGHT - 12 - scrollBarPosition : (DISPLAY_HEIGHT - 13) / (numItems < 1 ? 1 : numItems);

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
  if (numItems < 1)
  {
    if (ClickButtonEnc.clicks == 1)
      screenManager.back();
    return;
  }

  if (ClickButtonEnc.clicks != 0)
  {
    ESP_LOGI(TAG, "Clicks: %d", ClickButtonEnc.clicks);
    items[active]->run();
  }
  
  MenuItem *currentItem = items[active];

  if (encoderGetCount() > 0)
  {
    // Check if the current item is a Number or a Select and is in edit mode.
    if ((currentItem->getType() == MenuItemType::Number) ||
        (currentItem->getType() == MenuItemType::Select))
    {
      // Try casting to MenuItemNumberBase or MenuItemSelect.
      if (currentItem->getType() == MenuItemType::Select)
      {
        auto selectItem = dynamic_cast<MenuItemSelect *>(currentItem);
        if (selectItem && selectItem->isSelected())
          selectItem->nextOption();
        else
          nextItem();
      }
      else // Number type
      {
        auto numberItem = (MenuItemNumberBase *)currentItem;
        if (numberItem && numberItem->isSelected())
          numberItem->increase();
        else
          nextItem();
      }
    }
    else
      nextItem();

    encoderClearCount();
  }
  else if (encoderGetCount() < 0)
  {
    if ((currentItem->getType() == MenuItemType::Number) ||
        (currentItem->getType() == MenuItemType::Select))
    {
      if (currentItem->getType() == MenuItemType::Select)
      {
        auto selectItem = dynamic_cast<MenuItemSelect *>(currentItem);
        if (selectItem && selectItem->isSelected())
          selectItem->prevOption();
        else
          prevItem();
      }
      else
      {
        auto numberItem = (MenuItemNumberBase *)currentItem;
        if (numberItem && numberItem->isSelected())
          numberItem->decrease();
        else
          prevItem();
      }
    }
    else
      prevItem();

    encoderClearCount();
  }
}