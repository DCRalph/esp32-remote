#include "SystemInfoScreen.h"

#include "config.h"
#include "Display.h"
#include "IO/Buttons.h"
#include "ScreenManager.h"

namespace SystemInfoScreenNamespace
{

  static String formatBytes(size_t bytes)
  {
    if (bytes < 1024)
    {
      return String(bytes) + " B";
    }
    else if (bytes < (1024 * 1024))
    {
      return String(bytes / 1024.0, 2) + " KB";
    }
    else
    {
      return String(bytes / 1024.0 / 1024.0, 2) + " MB";
    }
  }

  void draw()
  {
    display.setTextDatum(TL_DATUM);

    display.setTextSize(2);

    display.setTextColor(TFT_WHITE);
    display.drawString("Heap Size:", 10, 30);
    display.setTextColor(TFT_CYAN);
    display.drawString(formatBytes(ESP.getHeapSize()), 150, 30);

    display.setTextColor(TFT_WHITE);
    display.drawString("Free Heap:", 10, 50);
    display.setTextColor(TFT_CYAN);
    display.drawString(formatBytes(ESP.getFreeHeap()), 150, 50);

    display.setTextColor(TFT_WHITE);
    display.drawString("PSRAM Size:", 10, 110);
    display.setTextColor(TFT_CYAN);
    display.drawString(formatBytes(ESP.getPsramSize()), 150, 110);

    display.setTextColor(TFT_WHITE);
    display.drawString("Free PSRAM:", 10, 140);
    display.setTextColor(TFT_CYAN);
    display.drawString(formatBytes(ESP.getFreePsram()), 150, 140);
  }

  void update()
  {
    if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
      screenManager.back();
  }

} // namespace SystemInfoScreenNamespace

const Screen2 SystemInfoScreen = {
    .name = "System Info",
    .draw = SystemInfoScreenNamespace::draw,
    .update = SystemInfoScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
