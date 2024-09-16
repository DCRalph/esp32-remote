#pragma once

#include "config.h"
#include "IO/Display.h"
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
  display.sprite.setTextDatum(TL_DATUM); // Top left datum
  display.sprite.fillScreen(TFT_BLACK); // Clear the screen
  
  display.sprite.setTextSize(3);

  // Heap Size
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.drawString("Heap Size:", 10, 50);
  display.sprite.setTextColor(TFT_CYAN);
  display.sprite.drawString(formatBytes(ESP.getHeapSize()), 230, 50); // Position the value next to the label

  // Free Heap
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.drawString("Free Heap:", 10, 80);
  display.sprite.setTextColor(TFT_CYAN);
  display.sprite.drawString(formatBytes(ESP.getFreeHeap()), 230, 80);

  // PSRAM Size
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.drawString("PSRAM Size:", 10, 110);
  display.sprite.setTextColor(TFT_CYAN);
  display.sprite.drawString(formatBytes(ESP.getPsramSize()), 230, 110);

  // Free PSRAM
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.drawString("Free PSRAM:", 10, 140);
  display.sprite.setTextColor(TFT_CYAN);
  display.sprite.drawString(formatBytes(ESP.getFreePsram()), 230, 140);

  
  // Push the sprite to the display
  display.sprite.pushSprite(0, 0);
}

void SystemInfoScreen::update()
{
  // If any button is clicked, go back to the previous screen
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.back();
}
