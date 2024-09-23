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

  sprite.fillRect(0, 0, LCD_WIDTH, 20, screenManager.getScreenTopBarColor());

  sprite.setTextSize(2);
  sprite.setTextColor(screenManager.getScreenTopBarTextColor());
  sprite.setTextDatum(ML_DATUM);

  String name = screenManager.getCurrentScreen()->name;
  // if (name.length() > 8)
  //   name = name.substring(0, 5) + "...";

  sprite.drawString(name, 10, 10);
  char buf[20];
  sprintf(buf, "%i%%", battery.getPercentageI());

  sprite.setTextDatum(MR_DATUM);
  sprite.drawString(buf, LCD_WIDTH - 10, 10);
}

void Display::clearScreen(u16_t color)
{
  sprite.fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

void Display::wrapTextInBounds(String text, int x, int y, int width, int height, int textSize, int gap, u16_t color, int textAlignment)
{
  sprite.setTextSize(textSize);
  sprite.setTextColor(color);
  sprite.setTextDatum(textAlignment);

  String wrappedText = "";
  String word = "";
  int lines = 1;

  // sprite.drawRect(x, y, width, height, TFT_RED);

  for (int i = 0; i < text.length(); i++)
  {
    char c = text.charAt(i);

    if (c == ' ')
    {
      String currentLine = wrappedText.substring(wrappedText.lastIndexOf('\n') + 1);
      int currentLineWidth = sprite.textWidth(currentLine) + sprite.textWidth(word);
      if (currentLineWidth > width)
      {
        wrappedText += "\n";
        lines++;
      }

      wrappedText += word + " ";
      word = "";
    }
    else
      word += c;
  }

  String currentLine = wrappedText.substring(wrappedText.lastIndexOf('\n') + 1);
  int currentLineWidth = sprite.textWidth(currentLine) + sprite.textWidth(word);
  if (currentLineWidth > width)
  {
    wrappedText += "\n" + word;
    lines++;
  }
  else
    wrappedText += word;

  for (int i = 0; i < lines; i++)
  {
    String line = wrappedText.substring(0, wrappedText.indexOf('\n'));
    wrappedText = wrappedText.substring(wrappedText.indexOf('\n') + 1);

    sprite.drawString(line, x, y + i * (sprite.fontHeight() + gap));
  }
}

void Display::noTopBar(void)
{
  showMenuBar = false;
}

void Display::push(void)
{
  sprite.pushSprite(0, 0);
}

void Display::display(void)
{
  clearScreen();

  screenManager.draw();
  if (showMenuBar)
    drawTopBar();
  showMenuBar = true;

  if (screenManager.isPopupActive())
    screenManager.drawPopup();
  else
    screenManager.update();

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