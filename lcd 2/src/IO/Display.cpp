#include "Display.h"

void Display::init(void)
{
  sprite.createSprite(LCD_WIDTH, LCD_HEIGHT);
  sprite.setSwapBytes(1);

  tft.begin();
  tft.setRotation(TFT_ROT);

  clearScreen();
  push();
}

void Display::drawTopBar()
{

  // bool wifiConnected = WiFi.status() == WL_CONNECTED;
  bool wifiConnected = false;

  sprite.fillSmoothRoundRect(0, 0, LCD_WIDTH, 20, 20, TFT_BLUE);

  sprite.setTextSize(2);
  sprite.setTextColor(TFT_BLACK);
  sprite.setTextDatum(ML_DATUM);

  String name = screenManager.getCurrentScreen()->name;
  // if (name.length() > 8)
  //   name = name.substring(0, 5) + "...";

  sprite.drawString(name, 10, 10);
  char buf[20];
  // sprintf(buf, "%i %.1fV", wifiConnected, battery.getVoltage());
  // sprintf(buf, "%i %.2f%%", wifiConnected, battery.getPercentageF());
  // sprintf(buf, "%i %i%%", wifiConnected, battery.getPercentageI());
  // sprintf(buf, "%i %i%%", wifiConnected, battery.getPercentage10());
  sprintf(buf, "%i%%", battery.getPercentageI());

  sprite.setTextDatum(MR_DATUM);
  sprite.drawString(buf, LCD_WIDTH - 10, 10);
}

void Display::clearScreen(u16_t color)
{
  sprite.fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

void Display::noTopBar(void)
{
  showMenuBar = false;
}

void Display::push(void)
{
  // lcd_PushColors(0, 0, LCD_WIDTH, LCD_HEIGHT, (u16_t *)sprite.getPointer());
  sprite.pushSprite(0, 0);
}

void Display::display(void)
{
  clearScreen();

  screenManager.draw();
  screenManager.update();

  if (showMenuBar)
    drawTopBar();
  showMenuBar = true;

  push();
}

//****************************************************

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