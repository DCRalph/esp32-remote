#include "Display.h"
#include "Wireless.h"

Display::Display()
{
}

void Display::init(void)
{
  Serial.println("\t[INFO] [Display] Initializing...");
  u8g2.begin();

  Serial.println("\t[INFO] [Display] Initialized");
}

void Display::drawCenteredText(uint8_t y, String text)
{
  u8g2.drawStr((DISPLAY_WIDTH - u8g2.getStrWidth(text.c_str())) / 2, y, text.c_str());
}

void Display::drawTopBar(void)
{
  u8g2.setFont(u8g2_font_koleeko_tf);
  u8g2.setDrawColor(1);
  u8g2.drawStr(0, 9, screenManager.getCurrentScreen()->topBarText.c_str());
  u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  char buffer[32];

  sprintf(buffer, "%d%%%", batteryGetPercentageSmooth());

  u8g2.setFont(u8g2_font_koleeko_tf);
  int battW = u8g2.getStrWidth(buffer);
  u8g2.drawStr(DISPLAY_WIDTH - battW, 9, buffer);

  u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
  if (WiFi.status() == WL_CONNECTED)
    u8g2.drawGlyph(DISPLAY_WIDTH - 8 - battW - 2, 9, 0x00f8);
  else if (wm.getConfigPortalActive() || wm.getWebPortalActive())
    u8g2.drawGlyph(DISPLAY_WIDTH - 8 - battW - 2, 9, 0x0081);
  else if (wireless.isSetupDone())
    u8g2.drawGlyph(DISPLAY_WIDTH - 8 - battW - 2, 9, 0x00c6);
  else
    u8g2.drawGlyph(DISPLAY_WIDTH - 8 - battW - 2, 9, 0x0079);
}

void Display::noTopBar()
{
  _noTopBar = true;
}

void Display::display(void)
{

  u8g2.firstPage();
  do
  {
    screenManager.draw();
    if (!_noTopBar)
      drawTopBar();
  } while (u8g2.nextPage());

  screenManager.update();
  _noTopBar = false;
}

Screen::Screen(String _name)
{
  name = _name;
  topBarText = _name;
}

Screen::Screen(String _name, String _topBarText)
{
  name = _name;
  topBarText = _topBarText;
}

void Screen::setTopBarText(String _text)
{
  topBarText = _text;
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