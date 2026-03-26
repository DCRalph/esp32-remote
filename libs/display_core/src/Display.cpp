// Display facade implementation.
#include "Display.h"
#include "ScreenManager.h"

Display::Display() : config(), screenManager(nullptr), showMenuBar(true) {}

void Display::begin(const DisplayConfig &config, ScreenManager *screenManager)
{
  this->config = config;
  this->screenManager = screenManager;

  if (this->config.ops.init)
    this->config.ops.init(this->config.context, this->config);

  clearScreen();
  push();
}

void Display::setScreenManager(ScreenManager *screenManager)
{
  this->screenManager = screenManager;
}

bool Display::isReady() const
{
  return config.context != nullptr || config.ops.push != nullptr;
}

uint16_t Display::width() const
{
  return config.width;
}

uint16_t Display::height() const
{
  return config.height;
}

uint8_t Display::rotation() const
{
  return config.rotation;
}

uint8_t Display::colorDepth() const
{
  return config.colorDepth;
}

void Display::clearScreen(uint16_t color)
{
  if (config.ops.clear)
    config.ops.clear(config.context, color);
}

void Display::fillScreen(uint16_t color)
{
  if (config.ops.fillScreen)
    config.ops.fillScreen(config.context, color);
  else if (config.ops.clear)
    config.ops.clear(config.context, color);
}

void Display::push()
{
  if (config.ops.push)
    config.ops.push(config.context);
}

void Display::setTextSize(uint8_t size)
{
  if (config.ops.setTextSize)
    config.ops.setTextSize(config.context, size);
}

void Display::setTextDatum(uint8_t datum)
{
  if (config.ops.setTextDatum)
    config.ops.setTextDatum(config.context, datum);
}

void Display::setTextColor(uint16_t color)
{
  if (config.ops.setTextColor)
    config.ops.setTextColor(config.context, color);
}

void Display::setTextColor(uint16_t fg, uint16_t bg)
{
  if (config.ops.setTextColorBg)
    config.ops.setTextColorBg(config.context, fg, bg);
  else if (config.ops.setTextColor)
    config.ops.setTextColor(config.context, fg);
}

void Display::drawString(const String &text, int32_t x, int32_t y)
{
  if (config.ops.drawString)
    config.ops.drawString(config.context, text, x, y);
}

void Display::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
{
  if (config.ops.drawLine)
    config.ops.drawLine(config.context, x0, y0, x1, y1, color);
}

void Display::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
  if (config.ops.drawRect)
    config.ops.drawRect(config.context, x, y, w, h, color);
}

void Display::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
  if (config.ops.fillRect)
    config.ops.fillRect(config.context, x, y, w, h, color);
}

void Display::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
{
  if (config.ops.drawRoundRect)
    config.ops.drawRoundRect(config.context, x, y, w, h, r, color);
}

void Display::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
{
  if (config.ops.fillRoundRect)
    config.ops.fillRoundRect(config.context, x, y, w, h, r, color);
}

void Display::drawSmoothArc(int32_t x, int32_t y, int32_t r, int32_t ir, int32_t startAngle, int32_t endAngle, uint16_t fgColor, uint16_t bgColor, bool roundEnds)
{
  if (config.ops.drawSmoothArc)
    config.ops.drawSmoothArc(config.context, x, y, r, ir, startAngle, endAngle, fgColor, bgColor, roundEnds);
}

int16_t Display::textWidth(const String &text)
{
  if (!config.ops.textWidth)
    return 0;
  return config.ops.textWidth(config.context, text);
}

int16_t Display::fontHeight()
{
  if (!config.ops.fontHeight)
    return 0;
  return config.ops.fontHeight(config.context);
}

uint16_t Display::color565(uint8_t r, uint8_t g, uint8_t b)
{
  if (!config.ops.color565)
    return 0;
  return config.ops.color565(config.context, r, g, b);
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
  if (!screenManager || !screenManager->getCurrentScreen())
    return;

  fillRect(0, 0, width(), 20, screenManager->getScreenTopBarColor());

  setTextSize(2);
  setTextColor(screenManager->getScreenTopBarTextColor());
  setTextDatum(ML_DATUM);

  drawString(String(screenManager->getCurrentScreen()->name), 10, 10);
  char buf[20];
  sprintf(buf, "%i%%", getBatteryPercentage());

  setTextDatum(MR_DATUM);
  drawString(buf, width() - 10, 10);
}

void Display::render(ScreenManager &manager)
{
  setScreenManager(&manager);
  render();
}

void Display::render()
{
  if (!screenManager)
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

Display display;
