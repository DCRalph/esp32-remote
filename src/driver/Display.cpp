#include "driver/Display.h"

void Display::init(void)
{
  sprite.createSprite(LCD_WIDTH, LCD_HEIGHT);
  sprite.setSwapBytes(1);

  rm67162_init(); // amoled lcd initialization
  lcd_setRotation(TFT_ROT);
  lcd_brightness(255);

  // logger.println("\tTFT Display initialized");
}

void Display::drawTopBar()
{
  sprite.fillSmoothRoundRect(0, 0, LCD_WIDTH, 30, 20, TFT_ORANGE);

  sprite.setTextSize(3);
  sprite.setTextColor(TFT_BLACK);
  sprite.setTextDatum(ML_DATUM);

  sprite.drawString(screenManager.getCurrentScreenName(), 10, 15);
  char buf[20];
  sprintf(buf, "%.1fV", battery.getVoltage());
  sprite.setTextDatum(MR_DATUM);
  sprite.drawString(buf, LCD_WIDTH - 10, 15);
}

void Display::clearScreen(u16_t color)
{
  sprite.fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

void Display::push(void)
{
  lcd_PushColors(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint16_t *)sprite.getPointer());
}

void Display::display(void)
{
  clearScreen();

  screenManager.draw();
  screenManager.update();

  drawTopBar();
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