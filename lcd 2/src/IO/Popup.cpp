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
