#include "Popup.h"
#include "MenuInput.h"
#include "ScreenManager.h"

Popup::Popup(String _title, String _message) : title(_title), message(_message)
{
}

void Popup::draw()
{
  int maxX = display.width();
  int maxY = display.height();

  display.fillRoundRect(30, 20, maxX - 60, maxY - 40, 10, TFT_WHITE);

  display.setTextSize(3);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(TL_DATUM);
  display.drawString(title, 40, 25);

  display.wrapTextInBounds(message, 40, 55, maxX - 80, maxY - 80, 2, 3, TFT_BLACK, TL_DATUM);
}

void Popup::update()
{
  MenuInput::update();
  if (MenuInput::getSelectClicks() != 0 || MenuInput::getBackClicks() != 0)
  {
    screenManager.closePopup();
  }
}

AutoClosePopup::AutoClosePopup(String _title, String _message, unsigned long _duration)
    : Popup(_title, _message), duration(_duration)
{
  startTime = millis();
}

void AutoClosePopup::draw()
{
  int maxX = display.width();
  int maxY = display.height();

  display.fillRoundRect(30, 20, maxX - 60, maxY - 40, 10, TFT_WHITE);

  display.fillRoundRect(30, 5, maxX - 60, 10, 5, TFT_WHITE);
  display.fillRoundRect(32, 8, progressBarWidth, 4, 2, TFT_RED);

  display.setTextSize(3);
  display.setTextColor(TFT_BLUE);
  display.setTextDatum(TL_DATUM);
  display.drawString(title, 40, 25);

  display.wrapTextInBounds(message, 40, 55, maxX - 80, maxY - 80, 2, 3, TFT_BLACK, TL_DATUM);
}

void AutoClosePopup::update()
{
  if (millis() - startTime > duration)
  {
    screenManager.closePopup();
    return;
  }

  progressBarWidth = map(millis() - startTime, 0, duration, display.width() - 64, 0);
}
