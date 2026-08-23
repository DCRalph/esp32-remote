#include "IO/TopBar.h"

#include <dcr_NetLink.h>

#include "config.h"
#include "IO/Battery.h"
#include "IO/U8g2DisplayDriver.h"

extern U8g2DisplayDriverContext displayDriverContext;

namespace
{
  constexpr int kIconWidth = 8;
  constexpr int kIconGap = 2;

  // Glyphs from u8g2_font_open_iconic_all_1x_t.
  constexpr uint16_t kGlyphWifi = 0x00f8;
  constexpr uint16_t kGlyphConnecting = 0x0081;
  constexpr uint16_t kGlyphBroadcast = 0x00c6;
  constexpr uint16_t kGlyphOffline = 0x0079;

  // Screens draw before the bar does, so the first frame uses this seed and
  // every frame after uses the position `render` left behind.
  int gNextIconX = DISPLAY_WIDTH;

  uint16_t linkGlyph()
  {
    if (WiFi.status() == WL_CONNECTED)
      return kGlyphWifi;
    if (wireless.isSetupDone())
      return kGlyphBroadcast;
    if (netLink.state() == WiFiState::WiFiConnecting)
      return kGlyphConnecting;
    return kGlyphOffline;
  }
}

void TopBar::render(Display &display, const Screen2 *screen)
{
  auto &u8g2 = displayDriverContext.u8g2;
  const int maxX = display.width();

  char battery[16];
  snprintf(battery, sizeof(battery), "%d%%", batteryGetPercentageSmooth());

  display.setTextSize(U8G2_TEXT_BAR);
  display.setTextColor(TFT_WHITE);

  display.setTextDatum(TL_DATUM);
  display.drawString(screen->name, 0, 0);

  display.setTextDatum(TR_DATUM);
  display.drawString(battery, maxX, 0);

  const int batteryWidth = display.textWidth(battery);
  const int iconX = maxX - batteryWidth - kIconGap - kIconWidth;

  // No glyph primitive on the facade, so reach through to u8g2 for the icon.
  u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
  u8g2.setFontPosTop();
  u8g2.setDrawColor(1);
  u8g2.drawGlyph(iconX, 0, linkGlyph());

  display.drawLine(0, kHeight - 2, maxX, kHeight - 2, TFT_WHITE);

  gNextIconX = iconX;
}

int TopBar::nextIconX(int width)
{
  gNextIconX -= kIconGap + width;
  return gNextIconX;
}
