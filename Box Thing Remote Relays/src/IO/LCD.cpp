#include "LCD.h"

Screen::Screen(const char *name, void (*draw)(), void (*update)(), void (*onEnter)(), void (*onExit)())
    : name(name), draw(draw), update(update), onEnter(onEnter), onExit(onExit)
{
}

LCD::LCD() : lcd(0x27, 16, 2)
{
}

void LCD::init()
{
  lcd.init();
  Wire.setClock(200000);

  lcd.createChar(0, LCD_cross);
  lcd.createChar(1, LCD_check);

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Detonator");
}

void LCD::loop()
{
  if (millis() - lastDraw < 100)
  {
    return;
  }
  lastDraw = millis();

  if (currentScreen != nullptr)
  {
    currentScreen->draw();
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No screen set");
  }
}

void LCD::setScreen(Screen *screen)
{
  if (currentScreen != nullptr && currentScreen->onExit != nullptr)
  {
    currentScreen->onExit();
  }

  currentScreen = screen;

  if (currentScreen != nullptr && currentScreen->onEnter != nullptr)
  {
    currentScreen->onEnter();
  }
}

LCD lcd;