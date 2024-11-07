#include "LCD.h"

LCD::LCD() : lcd(0x27, 16, 2)
{
}

void LCD::init()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Detonator");
}

void LCD::loop()
{
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