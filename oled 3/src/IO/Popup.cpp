#include "Popup.h"

Popup::Popup(String _title, String _message) : title(_title), message(_message)
{
}

void Popup::draw()
{
  display.sprite.fillRoundRect(30, 20, LCD_WIDTH - 60, LCD_HEIGHT - 40, 10, TFT_WHITE);

  display.sprite.setTextSize(3);
  display.sprite.setTextColor(TFT_BLACK);
  display.sprite.setTextDatum(TL_DATUM);
  display.sprite.drawString(title, 40, 25);

  display.wrapTextInBounds(message, 40, 55, LCD_WIDTH - 80, LCD_HEIGHT - 80, 2, 3, TFT_BLACK, TL_DATUM);
}

void Popup::update()
{
  if (ClickButtonUP.clicks != 0 || ClickButtonDOWN.clicks != 0)
  {
    screenManager.closePopup();
  }
}

AutoClosePopup::AutoClosePopup(String _title, String _message, unsigned long _duration) : Popup(_title, _message), duration(_duration)
{
  startTime = millis();
}

void AutoClosePopup::draw()
{
  display.sprite.fillRoundRect(30, 20, LCD_WIDTH - 60, LCD_HEIGHT - 40, 10, TFT_WHITE);

  display.sprite.fillRoundRect(30, 5, LCD_WIDTH - 60, 10, 5, TFT_WHITE);
  display.sprite.fillRoundRect(32, 8, progressBarWidth, 4, 2, TFT_RED);


  display.sprite.setTextSize(3);
  display.sprite.setTextColor(TFT_BLUE);
  display.sprite.setTextDatum(TL_DATUM);
  display.sprite.drawString(title, 40, 25);

  display.wrapTextInBounds(message, 40, 55, LCD_WIDTH - 80, LCD_HEIGHT - 80, 2, 3, TFT_BLACK, TL_DATUM);
}

void AutoClosePopup::update()
{
  if (millis() - startTime > duration)
  {
    screenManager.closePopup();
    return;
  }

  progressBarWidth = map(millis() - startTime, 0, duration, LCD_WIDTH - 64, 0);
}
