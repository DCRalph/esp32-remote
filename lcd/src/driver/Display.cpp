#include "driver/Display.h"

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

  bool wifiConnected = WiFi.status() == WL_CONNECTED;

  sprite.fillSmoothRoundRect(0, 0, LCD_WIDTH, 20, 20, TFT_BLUE);

  sprite.setTextSize(2);
  sprite.setTextColor(TFT_BLACK);
  sprite.setTextDatum(ML_DATUM);

  String name = screenManager.getCurrentScreenName();
  // if (name.length() > 8)
  //   name = name.substring(0, 5) + "...";

  sprite.drawString(name, 10, 10);
  char buf[20];
  // sprintf(buf, "%i %.1fV", wifiConnected, battery.getVoltage());
  // sprintf(buf, "%i %.2f%%", wifiConnected, battery.getPercentageF());
  sprintf(buf, "%i %i%%", wifiConnected, battery.getPercentageI());
  // sprintf(buf, "%i %i%%", wifiConnected, battery.getPercentage10());
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

Screen::Screen(String _name, String _id)
{
  name = _name;
  id = _id;
}

void Screen::draw()
{
  display.sprite.setTextSize(3);
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.setTextDatum(MC_DATUM);

  display.sprite.drawString("SCREEN!!!", LCD_WIDTH / 2, LCD_HEIGHT / 2);

  char buf[50];
  sprintf(buf, "Sec: %i", millis() / 1000);

  display.sprite.drawString(buf, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);
}

void Screen::update()
{
}

Display display;