#include "Menu.h"
#include "MenuInput.h"
#include "ScreenManager.h"

static const char *TAG = "Menu";

namespace
{
  constexpr uint16_t kMenuBg = TFT_BLACK;
  constexpr uint16_t kMenuText = TFT_WHITE;
  constexpr uint16_t kMenuActiveBg = TFT_WHITE;
  constexpr uint16_t kMenuActiveText = TFT_BLACK;

  int calcTextY(int y, const MenuStyle::DrawStyle &style)
  {
    return y + (style.lineHeight - display.fontHeight()) / 2;
  }
} // namespace

void MenuItem::draw(uint8_t _x, uint8_t _y, bool _active)
{
  int maxX = display.width();
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuStyle::DrawStyle style = MenuStyle::drawStyle(size);

  display.setTextSize(style.textSize);
  display.setTextDatum(TL_DATUM);

  if (_active)
  {
    display.fillRoundRect(_x, _y, maxX - 4, style.lineHeight, style.radius, bgColor);
    display.setTextColor(activeTextColor, bgColor);
  }
  else
  {
    display.setTextColor(textColor, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.drawString(getName(), _x + 2, textY);
}

void MenuItemToggle::draw(uint8_t _x, uint8_t _y, bool _active)
{
  int maxX = display.width();
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuStyle::DrawStyle style = MenuStyle::drawStyle(size);

  display.setTextSize(style.textSize);
  display.setTextDatum(TL_DATUM);

  if (_active)
  {
    display.fillRoundRect(_x, _y, maxX - 4, style.lineHeight, style.radius, bgColor);
    display.setTextColor(activeTextColor, bgColor);
  }
  else
  {
    display.setTextColor(textColor, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.drawString(getName(), _x + 2, textY);

  String valueStr = *value ? "ON" : "OFF";
  display.setTextDatum(TR_DATUM);
  display.drawString(valueStr, maxX - 6, textY);
}

void MenuItemString::draw(uint8_t _x, uint8_t _y, bool _active)
{
  int maxX = display.width();
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuStyle::DrawStyle style = MenuStyle::drawStyle(size);

  display.setTextSize(style.textSize);
  display.setTextDatum(TL_DATUM);

  if (_active)
  {
    display.fillRoundRect(_x, _y, maxX - 4, style.lineHeight, style.radius, bgColor);
    display.setTextColor(activeTextColor, bgColor);
  }
  else
  {
    display.setTextColor(textColor, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.drawString(getName(), _x + 2, textY);
  display.setTextDatum(TR_DATUM);
  display.drawString(getValue(), maxX - 6, textY);
}

template <typename T>
void MenuItemNumber<T>::draw(uint8_t _x, uint8_t _y, bool _active)
{
  int maxX = display.width();
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuStyle::DrawStyle style = MenuStyle::drawStyle(size);

  if (!_active && selected)
  {
    selected = false;
  }

  display.setTextSize(style.textSize);
  display.setTextDatum(TL_DATUM);

  String valueStr = String(*value);

  if (_active && selected)
  {
    display.drawRoundRect(_x, _y, maxX - 4, style.lineHeight, style.radius, textColor);
    display.setTextColor(textColor, kMenuBg);
  }
  else if (_active)
  {
    display.fillRoundRect(_x, _y, maxX - 4, style.lineHeight, style.radius, bgColor);
    display.setTextColor(activeTextColor, bgColor);
  }
  else
  {
    display.setTextColor(textColor, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.drawString(getName(), _x + 2, textY);

  display.setTextDatum(TR_DATUM);
  display.drawString(valueStr, maxX - 6, textY);
}

void MenuItemSelect::draw(uint8_t _x, uint8_t _y, bool _active)
{
  int maxX = display.width();
  MenuSize size = parent ? parent->getMenuSize() : MenuSize::Large;
  MenuStyle::DrawStyle style = MenuStyle::drawStyle(size);
  String dispText = getName();
  String optionText = getSelectedOption();

  display.setTextSize(style.textSize);
  display.setTextDatum(TL_DATUM);

  if (_active && selected)
  {
    display.drawRoundRect(_x, _y, maxX - 4, style.lineHeight, style.radius, textColor);
    display.setTextColor(textColor, kMenuBg);
  }
  else if (_active)
  {
    display.fillRoundRect(_x, _y, maxX - 4, style.lineHeight, style.radius, bgColor);
    display.setTextColor(activeTextColor, bgColor);
  }
  else
  {
    display.setTextColor(textColor, kMenuBg);
  }

  int textY = calcTextY(_y, style);
  display.drawString(dispText, _x + 2, textY);
  display.setTextDatum(TR_DATUM);
  display.drawString(optionText, maxX - 6, textY);
}

void Menu::draw()
{
  syncItemsPerPageWithDisplay();

  int maxX = display.width();
  int maxY = display.height();
  if (activeSubmenu)
  {
    activeSubmenu->draw();
    return;
  }

  MenuStyle::DrawStyle style = MenuStyle::drawStyle(menuSize);
  int lineHeight = style.lineHeight;
  int startY = MenuStyle::kContentTopY;

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

  if (numItemsVisible == 0)
  {
    return;
  }

  uint8_t itemRelIdx = calcActive - topItem;

  if (itemRelIdx <= 0 && calcActive == 0)
    topItem = 0;

  if (itemRelIdx <= 0 && calcActive > 0)
    topItem--;

  if (itemRelIdx >= numItemsPerPage - 1 && calcActive < numItemsVisible - 1)
    topItem++;

  if (itemRelIdx >= numItemsPerPage - 1 && calcActive == numItemsVisible - 1)
    topItem = numItemsVisible - numItemsPerPage;

  for (uint8_t i = 0; i < numItemsPerPage; i++)
  {
    uint8_t itemIdx = i + topItem;

    if (itemIdx >= numItemsVisible)
    {
      break;
    }

    MenuItem *item = items[itemMap[itemIdx]];

    item->draw(0, startY + (i * lineHeight), active == itemMap[itemIdx]);
  }

  int trackStart = startY;
  int trackHeight = maxY - trackStart - 1;
  int visibleCount = numItemsVisible < 1 ? 1 : numItemsVisible;
  int scrollBarPosition = (trackHeight - 1) / visibleCount * calcActive;
  int scrollBarHeight = (visibleCount - 1 == calcActive) ? trackHeight - scrollBarPosition : (trackHeight - 1) / visibleCount;

  display.drawLine(maxX - 2, trackStart, maxX - 2, maxY - 1, kMenuText);
  display.fillRect(maxX - 3, trackStart + scrollBarPosition, 3, scrollBarHeight, kMenuText);
}

void MenuItem::run()
{
  ESP_LOGI(TAG, "Running %s", name.c_str());
  ESP_LOGI(TAG, "Functions: %d", functions.size());

  int selectClicksForMatch = MenuInput::getSelectClicksForMatch();
  int rawSelectClicks = MenuInput::getSelectClicks();

  for (uint8_t i = 0; i < functions.size(); i++)
  {
    int want = functions[i].clicksToRun;
    bool match = (selectClicksForMatch == want) || (rawSelectClicks == want);
    if (match)
    {
      functions[i].func();
      break;
    }
  }
}

void Menu::update()
{
  MenuInput::update();

  if (activeSubmenu)
  {
    activeSubmenu->update();
    return;
  }

  if (MenuInput::getBackClicks() > 0)
  {
    if (getParentMenu())
      getParentMenu()->clearActiveSubmenu();
    else
      screenManager.back();
    return;
  }

  int selectClicks = MenuInput::getSelectClicks();
  int selectClicksForMatch = MenuInput::getSelectClicksForMatch();

  if (numItems < 1)
  {
    if (selectClicksForMatch == 1)
    {
      screenManager.back();
    }

    return;
  }

  if (selectClicks != 0)
  {
    items[active]->run();
  }

  MenuItem *currentItem = items[active];
  int encoderDelta = MenuInput::getNavigationDelta();

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
