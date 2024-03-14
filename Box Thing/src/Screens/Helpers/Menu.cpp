#include "Menu.h"

// ###### MenuItem ######

MenuItem::MenuItem(String _name, std::function<void()> _func)
{
  name = _name;
  func = _func;
}

void MenuItem::setName(String _name)
{
  name = _name;
}

String MenuItem::getName()
{
  return name;
}

void MenuItem::setFunction(std::function<void()> _func)
{
  func = _func;
}

void MenuItem::setClicksToRun(s8 _clicksToRun)
{
  clicksToRun = _clicksToRun;
}

s8 MenuItem::getClicksToRun()
{
  return clicksToRun;
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
  display.u8g2.drawStr(_x + 1, _y + 15, name.c_str());
}

void MenuItem::run()
{
  func();
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
    u8 topItem = active - 1 < 0 ? 0 : active - 1 >= static_cast<u8>(numItems - 2) ? numItems < 3 ? 0 : static_cast<u8>(numItems - 3)
                                                                                  : active - 1;
    MenuItem *item = items[i + topItem];

    // if (active == i + topItem)
    // {
    //   display.u8g2.setDrawColor(1);
    //   display.u8g2.drawBox(0, 12 + (i * 18), DISPLAY_WIDTH - 4, 16);
    //   display.u8g2.setDrawColor(0);
    // }
    // else
    //   display.u8g2.setDrawColor(1);
    // display.u8g2.drawStr(1, 27 + (i * 18), item->getName().c_str());

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

  if (ClickButtonEnc.clicks == items[active]->getClicksToRun())
  {
    items[active]->run();
  }

  if (encoder.encoder.getPosition() > 0)
  {
    if (active < numItems - 1)
      active++;
    encoder.encoder.setPosition(0);
  }
  else if (encoder.encoder.getPosition() < 0)
  {
    if (active > 0)
      active--;
    encoder.encoder.setPosition(0);
  }
}