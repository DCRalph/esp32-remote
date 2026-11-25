// Live system stats on the touch panel, redrawn four times a second; the LCD
// picks the touch panel's theme.
#include "screens/Screens.h"

#include <WiFi.h>
#include <dcr_NetLink.h>

#include "config.h"
#include "Displays.h"
#include "IO/Battery.h"

namespace
{
  Theme lightTheme()
  {
    Theme t;
    t.background = Color::White;
    t.text = Color::Black;
    t.mutedText = Color::DarkGrey;
    t.accent = Color::Blue;
    t.highlight = Color::Blue;
    t.highlightText = Color::White;
    t.topBar = Color::DarkGrey;
    t.topBarText = Color::White;
    t.popupBackground = Color::LightGrey;
    t.widget = Color::LightGrey;
    t.widgetText = Color::Black;
    return t;
  }

  Theme amberTheme()
  {
    Theme t;
    t.text = Color::Orange;
    t.mutedText = Color::rgb(120, 70, 0);
    t.accent = Color::Orange;
    t.highlight = Color::Orange;
    t.topBar = Color::Orange;
    t.focus = Color::Orange;
    t.focusText = Color::Black;
    return t;
  }

  Menu menu(MenuSize::Medium);
  MenuItemBack backItem;
  MenuItemSelect themeItem("Touch theme", {"Dark", "Light", "Amber"});
  bool built = false;

  void lcdEnter()
  {
    if (!built)
    {
      built = true;
      menu.addMenuItem(&backItem);
      menu.addMenuItem(&themeItem);
      themeItem.setOnChange([]()
                            {
                              const int i = themeItem.getCurrentIndex();
                              touchDisplay.setTheme(i == 1 ? lightTheme() : i == 2 ? amberTheme() : Theme::dark()); });
    }
    touchScreens.setScreen(&Demos::DashboardTouch);
  }

  void lcdExit()
  {
    touchScreens.setScreen(&TouchHomeScreen);
  }

  // One labelled tile with an optional 0-100 bar.
  void tile(int col, int row, const String &label, const String &value, int percent = -1)
  {
    Display &d = touchDisplay;
    const int16_t w = (TOUCH_WIDTH - 18) / 2, h = 64;
    const int16_t x = 6 + col * (w + 6), y = 26 + row * (h + 6);
    d.drawRoundRect(x, y, w, h, 6, d.theme().mutedText);
    drawText(d, label, x + 8, y + 6, TextRole::Small, Datum::TopLeft, d.theme().mutedText);
    drawText(d, value, x + 8, y + 22, TextRole::Body);
    if (percent >= 0)
    {
      d.drawRect(x + 8, y + h - 14, w - 16, 8, d.theme().mutedText);
      d.fillRect(x + 9, y + h - 13, (w - 18) * constrain(percent, 0, 100) / 100, 6, d.theme().accent);
    }
  }

  void touchDraw()
  {
    const uint32_t heap = ESP.getFreeHeap(), heapTotal = ESP.getHeapSize();
    const uint32_t psram = ESP.getFreePsram(), psramTotal = ESP.getPsramSize();
    tile(0, 0, "Uptime", String(millis() / 1000) + " s");
    tile(1, 0, "Battery", String(battery.getVoltage(), 2) + " V", battery.getPercentageI());
    tile(0, 1, "Heap free", String(heap / 1024) + " KB", heapTotal ? heap * 100 / heapTotal : 0);
    tile(1, 1, "PSRAM free", String(psram / 1024) + " KB", psramTotal ? psram * 100 / psramTotal : 0);
    const FrameStats &lcd = display.stats(), &touch = touchDisplay.stats();
    tile(0, 2, "Frames LCD / touch", String(lcd.fps) + " / " + touch.fps + " fps", lcd.loadPercent);
    tile(1, 2, "Wi-Fi", netLink.isConnected() ? String(WiFi.RSSI()) + " dBm" : String("offline"),
         netLink.isConnected() ? constrain(2 * (WiFi.RSSI() + 100), 0, 100) : 0);
  }

  void touchUpdate()
  {
    for (const InputEvent &e : Inputs::events())
      if (e.kind == InputKind::Tap)
      {
        screenManager.back();
        return;
      }
  }
}

const Screen2 Demos::DashboardLcd = {
    .name = "Dashboard",
    .draw = []()
    { menu.draw(); },
    .update = []()
    { menu.update(); },
    .onEnter = lcdEnter,
    .onExit = lcdExit,
};

// Tap anywhere to leave.
const Screen2 Demos::DashboardTouch = makeScreen("Dashboard (tap to exit)", touchDraw, touchUpdate, nullptr, nullptr, 250);
