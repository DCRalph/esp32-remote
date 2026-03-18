#pragma once

#include "config.h"
#include "Display.h"
#include "IO/Buttons.h"

class SystemInfoScreen : public Screen
{
public:
  SystemInfoScreen(String _name) : Screen(_name){};

  void draw() override;
  void update() override;
};

// Helper function to convert bytes to a human-readable format (KB or MB)
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0, 2) + " KB";
  } else {
    return String(bytes / 1024.0 / 1024.0, 2) + " MB";
  }
}

void SystemInfoScreen::draw()
{
  display.setTextDatum(TL_DATUM); // Top left datum

  display.setTextSize(2);

  // Heap Size
  display.setTextColor(TFT_WHITE);
  display.drawString("Heap Size:", 10, 30);
  display.setTextColor(TFT_CYAN);
  display.drawString(formatBytes(ESP.getHeapSize()), 150, 30); // Position the value next to the label

  // Free Heap
  display.setTextColor(TFT_WHITE);
  display.drawString("Free Heap:", 10, 50);
  display.setTextColor(TFT_CYAN);
  display.drawString(formatBytes(ESP.getFreeHeap()), 150, 50);

  // PSRAM Size
  display.setTextColor(TFT_WHITE);
  display.drawString("PSRAM Size:", 10, 110);
  display.setTextColor(TFT_CYAN);
  display.drawString(formatBytes(ESP.getPsramSize()), 150, 110);

  // Free PSRAM
  display.setTextColor(TFT_WHITE);
  display.drawString("Free PSRAM:", 10, 140);
  display.setTextColor(TFT_CYAN);
  display.drawString(formatBytes(ESP.getFreePsram()), 150, 140);

}

void SystemInfoScreen::update()
{
  // If any button is clicked, go back to the previous screen
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.back();
}
