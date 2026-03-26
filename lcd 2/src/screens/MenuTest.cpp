#include "MenuTest.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Popup.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

namespace MenuTestScreenNamespace
{

  namespace
  {
    static Menu menu{MenuSize::Medium};
    static bool toggleA = false;
    static bool toggleB = true;
    static String statusText = "Ready";
    static int speed = 5;
    static uint8_t brightness = 50;
    static int nestedCount = 0;

    static Menu nested{MenuSize::Small};

    static MenuItemBack backItem;
    static MenuItemToggle toggleItemA{"Toggle A", &toggleA};
    static MenuItemToggle toggleItemB{"Toggle B", &toggleB};
    static MenuItemString statusItem{"Status", &statusText};
    static MenuItemNumber<int> speedItem{"Speed", &speed, 0, 10};
    static MenuItemNumber<uint8_t> brightnessItem{"Brightness", &brightness, 0, 100, 5};
    static MenuItemSelect colorSelect{"Color", {"Red", "Green", "Blue", "Cyan"}, 1};
    static MenuItemAction toastAction{"Show Popup", 1, []()
                                      { screenManager.showPopup(new AutoClosePopup("Popup", "Action fired", 1500)); }};

    static MenuItemBack nestedBack;
    static MenuItemAction nestedAction{"Nested Action", 1, []()
                                       { screenManager.showPopup(new AutoClosePopup("Nested", "Inside submenu", 1200)); }};
    static MenuItemNumber<int> nestedCounter{"Nested Count", &nestedCount, 0, 20};
    static MenuItemSubmenu submenuEntry{"More Options", &nested};

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      menu.addMenuItem(&backItem);
      menu.addMenuItem(&toggleItemA);
      menu.addMenuItem(&toggleItemB);
      menu.addMenuItem(&statusItem);
      menu.addMenuItem(&speedItem);
      menu.addMenuItem(&brightnessItem);
      menu.addMenuItem(&colorSelect);
      menu.addMenuItem(&toastAction);
      menu.addMenuItem(&submenuEntry);

      nested.addMenuItem(&nestedBack);
      nested.addMenuItem(&nestedAction);
      nested.addMenuItem(&nestedCounter);

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
  }

  void onEnter()
  {
    ensureInitialized();
    menu.setMenuSize(MenuSize::Medium);
  }

  void draw()
  {
    ensureInitialized();
    menu.draw();
  }

  void update()
  {
    ensureInitialized();
    menu.update();
  }

} // namespace MenuTestScreenNamespace

const Screen2 MenuTestScreen = {
    .name = "Menu Test",
    .draw = MenuTestScreenNamespace::draw,
    .update = MenuTestScreenNamespace::update,
    .onEnter = MenuTestScreenNamespace::onEnter,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
