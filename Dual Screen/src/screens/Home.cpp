#include "screens/Screens.h"

#include "Displays.h"

namespace
{
  Menu menu(MenuSize::Medium);
  MenuItemNavigate controlsItem("Shared controls", &Demos::ControlsLcd);
  MenuItemNavigate sketchItem("Sketch pad", &Demos::SketchLcd);
  MenuItemNavigate crossoverItem("Cross popups", &Demos::CrossoverLcd);
  MenuItemNavigate typingItem("Type to LCD", &Demos::TypingLcd);
  MenuItemNavigate dashboardItem("Dashboard", &Demos::DashboardLcd);
  MenuItemNavigate meshItem("Mesh with oled 3", &Demos::MeshLcd);
  MenuItemNavigate uiTestItem("UI test (LCD)", &UiTest::RootScreen);
  MenuItemNavigate settingsItem("Settings", &SettingsScreen);

  void enter()
  {
    if (!menu.items.empty())
      return;
    for (MenuItem *item : {static_cast<MenuItem *>(&controlsItem), static_cast<MenuItem *>(&sketchItem),
                           static_cast<MenuItem *>(&crossoverItem), static_cast<MenuItem *>(&typingItem),
                           static_cast<MenuItem *>(&dashboardItem), static_cast<MenuItem *>(&meshItem),
                           static_cast<MenuItem *>(&uiTestItem),
                           static_cast<MenuItem *>(&settingsItem)})
      menu.addMenuItem(item);
  }
}

void drawText(Display &d, const String &text, int16_t x, int16_t y, TextRole role, uint8_t datum, int32_t color)
{
  d.setTextRole(role);
  d.setTextColor(color == kThemeColor ? d.theme().text : static_cast<uint16_t>(color));
  d.setTextDatum(datum);
  d.drawString(text, x, y);
}

bool isExitEvent(const InputEvent &e)
{
  return e.kind == InputKind::Back || e.kind == InputKind::Left ||
         (e.kind == InputKind::Select && e.rawClicks == 3);
}

const Screen2 HomeScreen = {
    .name = "Dual Screen",
    .draw = []()
    { menu.draw(); },
    .update = []()
    { menu.update(); },
    .onEnter = enter,
    .onExit = nullptr,
};
