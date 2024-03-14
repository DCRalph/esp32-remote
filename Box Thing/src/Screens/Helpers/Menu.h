#pragma once

#include "config.h"
#include <vector>

#include "IO/Display.h"
#include "IO/GPIO.h"

class Menu;

// ###### MenuItem ######

class MenuItem
{
private:
  String name;
  std::function<void()> func;
  s8 clicksToRun = 1;

public:
  MenuItem(String _name, std::function<void()> _func);

  void setName(String _name);
  String getName();

  void setFunction(std::function<void()> _func);

  void setClicksToRun(s8 _clicksToRun);
  s8 getClicksToRun();

  void draw(u8 _x, u8 _y, bool _active);

  void run();
};

// ###### Menu ######

class Menu
{
private:
  u8 active;
  std::vector<MenuItem *> items;

  u8 maxItemsPerPage = 3;

  u8 numItems;
  u8 numItemsPerPage;

public:
  Menu();

  // String name;

  void setItemsPerPage(u8 _itemsPerPage);
  u8 getItemsPerPage();

  void setActive(u8 _active);
  u8 getActive();

  void addMenuItem(MenuItem *_item);
  void draw();
  void update();
};
