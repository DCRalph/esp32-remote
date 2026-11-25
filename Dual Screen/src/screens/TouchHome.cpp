#include "screens/Screens.h"

#include "Displays.h"

// Launches each demo through its LCD half, which opens the matching touch half.
namespace
{
  Menu menu(MenuSize::Large);
  MenuItemAction controlsItem("Shared controls", 1, []()
                              { screenManager.setScreen(&Demos::ControlsLcd); });
  MenuItemAction sketchItem("Sketch pad", 1, []()
                            { screenManager.setScreen(&Demos::SketchLcd); });
  MenuItemAction crossoverItem("Cross popups", 1, []()
                               { screenManager.setScreen(&Demos::CrossoverLcd); });
  MenuItemAction typingItem("Type to LCD", 1, []()
                            { screenManager.setScreen(&Demos::TypingLcd); });
  MenuItemAction dashboardItem("Dashboard", 1, []()
                               { screenManager.setScreen(&Demos::DashboardLcd); });
  MenuItemNavigate uiTestItem("UI test (touch)", &UiTest::RootScreen);

  void enter()
  {
    if (!menu.items.empty())
      return;
    for (MenuItem *item : {static_cast<MenuItem *>(&controlsItem), static_cast<MenuItem *>(&sketchItem),
                           static_cast<MenuItem *>(&crossoverItem), static_cast<MenuItem *>(&typingItem),
                           static_cast<MenuItem *>(&dashboardItem), static_cast<MenuItem *>(&uiTestItem)})
      menu.addMenuItem(item);
  }
}

const Screen2 TouchHomeScreen = {
    .name = "Touch",
    .draw = []()
    { menu.draw(); },
    .update = []()
    { menu.update(); },
    .onEnter = enter,
    .onExit = nullptr,
};
