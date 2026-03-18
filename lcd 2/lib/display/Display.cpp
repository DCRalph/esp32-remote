// Display facade implementation.
#include "Display.h"
#include "ScreenManager.h"

Display::Display() : driver(nullptr), screenManager(nullptr), showMenuBar(true) {}

void Display::begin(IDisplayDriver *driver, ScreenManager *screenManager)
{
  this->driver = driver;
  this->screenManager = screenManager;

  if (this->driver)
    this->driver->init();

  clearScreen();
  push();
}

void Display::setScreenManager(ScreenManager *screenManager)
{
  this->screenManager = screenManager;
}

bool Display::isReady() const
{
  return driver != nullptr;
}

void Display::clearScreen(uint16_t color)
{
  if (driver)
    driver->clear(color);
}

void Display::push()
{
  if (driver)
    driver->push();
}

void Display::setTextSize(uint8_t size)
{
  if (driver)
    driver->setTextSize(size);
}

void Display::setTextDatum(uint8_t datum)
{
  if (driver)
    driver->setTextDatum(datum);
}

void Display::setTextColor(uint16_t color)
{
  if (driver)
    driver->setTextColor(color);
}

void Display::setTextColor(uint16_t fg, uint16_t bg)
{
  if (driver)
    driver->setTextColor(fg, bg);
}

void Display::drawString(const String &text, int32_t x, int32_t y)
{
  if (driver)
    driver->drawString(text, x, y);
}

void Display::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
{
  if (driver)
    driver->drawLine(x0, y0, x1, y1, color);
}

void Display::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
  if (driver)
    driver->drawRect(x, y, w, h, color);
}

void Display::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
  if (driver)
    driver->fillRect(x, y, w, h, color);
}

void Display::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
{
  if (driver)
    driver->drawRoundRect(x, y, w, h, r, color);
}

void Display::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
{
  if (driver)
    driver->fillRoundRect(x, y, w, h, r, color);
}

int16_t Display::textWidth(const String &text)
{
  if (!driver)
    return 0;
  return driver->textWidth(text);
}

int16_t Display::fontHeight()
{
  if (!driver)
    return 0;
  return driver->fontHeight();
}

uint16_t Display::color565(uint8_t r, uint8_t g, uint8_t b)
{
  if (!driver)
    return 0;
  return driver->color565(r, g, b);
}

void Display::wrapTextInBounds(String text, int x, int y, int width, int height, int textSize, int gap, uint16_t color, int textAlignment)
{
  setTextSize(textSize);
  setTextColor(color);
  setTextDatum(textAlignment);

  String wrappedText = "";
  String word = "";
  int lines = 1;

  for (int i = 0; i < text.length(); i++)
  {
    char c = text.charAt(i);

    if (c == ' ')
    {
      String currentLine = wrappedText.substring(wrappedText.lastIndexOf('\n') + 1);
      int currentLineWidth = textWidth(currentLine) + textWidth(word);
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
  int currentLineWidth = textWidth(currentLine) + textWidth(word);
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

    drawString(line, x, y + i * (fontHeight() + gap));
  }
}

void Display::noTopBar(void)
{
  showMenuBar = false;
}

void Display::drawTopBar()
{
  if (!driver || !screenManager || !screenManager->getCurrentScreen())
    return;

  bool wifiConnected = false;
  (void)wifiConnected;

  fillRect(0, 0, LCD_WIDTH, 20, screenManager->getScreenTopBarColor());

  setTextSize(2);
  setTextColor(screenManager->getScreenTopBarTextColor());
  setTextDatum(ML_DATUM);

  String name = screenManager->getCurrentScreen()->name;

  drawString(name, 10, 10);
  char buf[20];
  sprintf(buf, "%i%%", getBatteryPercentage());

  setTextDatum(MR_DATUM);
  drawString(buf, LCD_WIDTH - 10, 10);
}

void Display::render(ScreenManager &manager)
{
  setScreenManager(&manager);
  render();
}

void Display::render()
{
  if (!driver || !screenManager)
    return;

  clearScreen();

  screenManager->draw();
  if (showMenuBar)
    drawTopBar();
  showMenuBar = true;

  if (screenManager->isPopupActive())
    screenManager->drawPopup();
  else
    screenManager->update();

  push();
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
