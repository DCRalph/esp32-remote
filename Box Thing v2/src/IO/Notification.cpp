#include "IO/Notification.h"

#include <Display.h>

#include "IO/U8g2DisplayDriver.h"

namespace
{
  String gMessage;
  uint64_t gStartTime = 0;
  uint32_t gDuration = 0;
  bool gActive = false;
}

void Notification::show(const String &message, uint32_t durationMs)
{
  gMessage = message;
  gStartTime = millis();
  gDuration = durationMs;
  gActive = true;
}

void Notification::hide()
{
  gActive = false;
  gMessage = "";
}

bool Notification::isActive()
{
  if (!gActive)
    return false;

  if (millis() - gStartTime >= gDuration)
  {
    hide();
    return false;
  }

  return true;
}

void Notification::draw()
{
  if (!isActive() || gMessage.isEmpty())
    return;

  display.setTextSize(U8G2_TEXT_SMALL);

  const int textWidth = display.textWidth(gMessage);
  const int textHeight = display.fontHeight();

  const int boxWidth = textWidth + 8;   // 4px padding each side
  const int boxHeight = textHeight + 6; // 3px padding top and bottom
  const int boxX = (display.width() - boxWidth) / 2;
  const int boxY = (display.height() - boxHeight) / 2;

  display.fillRect(boxX, boxY, boxWidth, boxHeight, TFT_WHITE);
  display.drawRect(boxX - 1, boxY - 1, boxWidth + 2, boxHeight + 2, TFT_WHITE);

  display.setTextColor(TFT_BLACK, TFT_WHITE);
  display.setTextDatum(TL_DATUM);
  display.drawString(gMessage, boxX + 4, boxY + 3);

  display.setTextColor(TFT_WHITE);
}
