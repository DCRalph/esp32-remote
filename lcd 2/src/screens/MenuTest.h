#pragma once

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Popup.h"
#include "IO/Buttons.h"

class MenuTestScreen : public Screen
{
public:
  MenuTestScreen(String _name);

  // Core menu
  Menu menu = Menu(MenuSize::Medium);

  // Demo state
  bool toggleA = false;
  bool toggleB = true;
  String statusText = "Ready";
  int speed = 5;
  uint8_t brightness = 50;
  int nestedCount = 0;

  // Submenu to exercise nested menus
  Menu nested = Menu(MenuSize::Small);

  // Menu items
  MenuItemBack backItem;
  MenuItemToggle toggleItemA = MenuItemToggle("Toggle A", &toggleA);
  MenuItemToggle toggleItemB = MenuItemToggle("Toggle B", &toggleB);
  MenuItemString statusItem = MenuItemString("Status", &statusText);
  MenuItemNumber<int> speedItem = MenuItemNumber<int>("Speed", &speed, 0, 10);
  MenuItemNumber<uint8_t> brightnessItem = MenuItemNumber<uint8_t>("Brightness", &brightness, 0, 100, 5);
  MenuItemSelect colorSelect = MenuItemSelect("Color", {"Red", "Green", "Blue", "Cyan"}, 1);
  MenuItemAction toastAction = MenuItemAction("Show Popup", 1, []()
                                              { screenManager.showPopup(new AutoClosePopup("Popup", "Action fired", 1500)); });

  // Nested menu items
  MenuItemBack nestedBack;
  MenuItemAction nestedAction = MenuItemAction("Nested Action", 1, []()
                                               { screenManager.showPopup(new AutoClosePopup("Nested", "Inside submenu", 1200)); });
  MenuItemNumber<int> nestedCounter = MenuItemNumber<int>("Nested Count", &nestedCount, 0, 20);
  MenuItemSubmenu submenuEntry = MenuItemSubmenu("More Options", &nested);

  void draw() override;
  void update() override;
  void onEnter() override;
};

MenuTestScreen::MenuTestScreen(String _name) : Screen(_name)
{
  // Main menu wiring
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&toggleItemA);
  menu.addMenuItem(&toggleItemB);
  menu.addMenuItem(&statusItem);
  menu.addMenuItem(&speedItem);
  menu.addMenuItem(&brightnessItem);
  menu.addMenuItem(&colorSelect);
  menu.addMenuItem(&toastAction);
  menu.addMenuItem(&submenuEntry);

  // Nested submenu wiring
  nested.addMenuItem(&nestedBack);
  nested.addMenuItem(&nestedAction);
  nested.addMenuItem(&nestedCounter);

  // On-change feedback
  toggleItemA.setOnChange([&]()
                          { statusText = toggleA ? "A ON" : "A OFF"; });
  toggleItemB.setOnChange([&]()
                          { statusText = toggleB ? "B ON" : "B OFF"; });
  speedItem.setOnChange([&]()
                        { statusText = "Speed " + String(speed); });
  brightnessItem.setOnChange([&]()
                             { statusText = "Bright " + String(brightness); });
  colorSelect.setOnChange([&]()
                          { statusText = "Color " + colorSelect.getSelectedOption(); });
  nestedCounter.setOnChange([&]()
                            { statusText = "Nested " + String(nestedCount); });
}

void MenuTestScreen::onEnter()
{
  // Ensure menu sizing recalculates with current items
  menu.setMenuSize(MenuSize::Medium);
}

void MenuTestScreen::draw()
{
  menu.draw();
}

void MenuTestScreen::update()
{
  menu.update();
}
