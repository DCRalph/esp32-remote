#include "Display.h"

Display::Display()
{
}

void Display::init(void)
{
  Serial.println("\t[INFO] [Display] Initializing...");
  u8g2.begin();

  Serial.println("\t[INFO] [Display] Initialized");
}

void Display::drawCenteredText(u8 y, String text)
{
  u8g2.drawStr((DISPLAY_WIDTH - u8g2.getStrWidth(text.c_str())) / 2, y, text.c_str());
}

void Display::refresh(void)
{
  refreshScreen = true;
}

void Display::display(void)
{
  // if (refreshScreen)
  // {
  //   u8g2.firstPage();
  //   do
  //   {
  //     screenManager.draw();
  //   } while (u8g2.nextPage());
  //   refreshScreen = false;
  // }

  u8g2.firstPage();
  do
  {
    screenManager.draw();
  } while (u8g2.nextPage());

  screenManager.update();
}

Screen::Screen(String _name)
{
  name = _name;
}

void Screen::draw()
{
}

void Screen::update()
{
}

void Screen::onEnter()
{
}

void Screen::onExit()
{
}

Display display;