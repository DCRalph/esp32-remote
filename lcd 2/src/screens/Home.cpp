#include "Home.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Popup.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

#include "Control.h"
#include "Control/CarLocks.h"
#include "MenuTest.h"
#include "Send.h"
#include "SettingsScreen.h"

namespace HomeScreenNamespace
{

  namespace
  {
    static Menu menu;
    static MenuItemNavigate controlMenuItem{"Control", &ControlScreen};
    static MenuItemNavigate settingsMenuItem{"Settings", &SettingsScreen};
    static MenuItemNavigate sendMenuItem{"Send", &SendScreen};
    static MenuItemNavigate menuTestItem{"Menu Test", &MenuTestScreen};
    static MenuItemAction testPopupItem{"Test Popup", 2, []()
                                        {
                                          screenManager.showPopup(new Popup("Test Popup", "This is a test gyY9 popup. pls wrap words TTTTT"));
                                        }};
    static MenuItemAction testPopupItem2{"Test Popup 2", 2, []()
                                         {
                                           screenManager.showPopup(new AutoClosePopup("Test Popup 2", "This is a test", 5000));
                                         }};

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      settingsMenuItem.setTextColor(TFT_RED);
      settingsMenuItem.setBgColor(TFT_RED);

      menu.addMenuItem(&controlMenuItem);
      controlMenuItem.addRoute(3, &CarLocksScreen);

      menu.addMenuItem(&settingsMenuItem);
      menu.addMenuItem(&sendMenuItem);
      menu.addMenuItem(&menuTestItem);
      menu.addMenuItem(&testPopupItem);
      menu.addMenuItem(&testPopupItem2);
    }
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

} // namespace HomeScreenNamespace

const Screen2 HomeScreen = {
    .name = "Home",
    .draw = HomeScreenNamespace::draw,
    .update = HomeScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
