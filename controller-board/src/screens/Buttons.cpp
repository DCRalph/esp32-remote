// The power and BOOT buttons through dcr_input: live state, the last click
// sequence, and BOOT's hold-to-repeat.
#include "screens/Screens.h"

#include "Device.h"
#include "config.h"

namespace
{
  struct Card
  {
    const char *title;
    Controls::Button &button;
    int lastClicks = 0;
    uint16_t repeats = 0;
  };

  Card power{"Power", Device::powerButton};
  Card boot{"BOOT (GPIO 0)", Device::bootButton};

  Button backButton("Back", []()
                    { screenManager.back(); });
  FocusGroup group;
  bool built = false;

  constexpr int16_t kMargin = 12;
  constexpr int16_t kCardY = 36;
  constexpr int16_t kCardW = (LCD_WIDTH - 3 * kMargin) / 2;
  constexpr int16_t kCardH = 130;

  String describe(int clicks)
  {
    if (clicks == 0)
      return "none yet";
    if (clicks == 1)
      return "1 click";
    if (clicks > 1)
      return String(clicks) + " clicks";
    if (clicks == -1)
      return "long press";
    return String(-clicks - 1) + " + long press";
  }

  void line(int16_t x, int16_t y, const char *label, const String &value)
  {
    const Theme &t = display.theme();
    display.setTextRole(TextRole::Small);
    display.setTextDatum(Datum::TopLeft);
    display.setTextColor(t.mutedText);
    display.drawString(label, x, y);
    display.setTextColor(t.text);
    display.drawString(value, x + 58, y);
  }

  void drawCard(int16_t x, const Card &card, const char *extraLabel, const String &extraValue)
  {
    const Theme &t = display.theme();
    const Controls::Button &b = card.button;
    Draw::raised(x, kCardY, kCardW, kCardH, MenuStyle::kGroupRadius, t.surface);
    display.drawRoundRect(x, kCardY, kCardW, kCardH, MenuStyle::kGroupRadius, t.border);

    display.setTextRole(TextRole::Body);
    display.setTextDatum(Datum::TopLeft);
    display.setTextColor(t.text);
    display.drawString(card.title, x + 10, kCardY + 10);

    const int16_t pillX = x + 10, pillY = kCardY + 36, pillW = kCardW - 20, pillH = 28;
    if (b.pressed())
      display.fillRoundRect(pillX, pillY, pillW, pillH, pillH / 2, t.accent);
    else
      display.drawRoundRect(pillX, pillY, pillW, pillH, pillH / 2, t.track);
    display.setTextDatum(Datum::MiddleCenter);
    display.setTextColor(b.pressed() ? t.onAccent : t.mutedText);
    display.drawString(b.pressed() ? "Pressed" : "Released", pillX + pillW / 2, pillY + pillH / 2);

    const String held = b.pressed() ? String(b.heldMs() / 1000.0f, 1) + " s" : String("no");
    line(x + 10, kCardY + 74, "Held", held);
    line(x + 10, kCardY + 92, "Last", describe(card.lastClicks));
    line(x + 10, kCardY + 110, extraLabel, extraValue);
  }

  void note(Card &card)
  {
    if (card.button.clicks() != 0)
      card.lastClicks = card.button.clicks();
    if (card.button.repeat())
      card.repeats++;
  }

  void enter()
  {
    power.lastClicks = boot.lastClicks = 0;
    boot.repeats = 0;
    if (built)
      return;
    built = true;
    backButton.bounds = {static_cast<int16_t>(LCD_WIDTH - 96), static_cast<int16_t>(LCD_HEIGHT - 32), 90, 28};
    group.add(backButton);
  }

  void draw()
  {
    drawCard(kMargin, power, "Off at", String(POWER_OFF_HOLD_MS / 1000) + " s held");
    drawCard(2 * kMargin + kCardW, boot, "Repeats", String(boot.repeats));

    const Theme &t = display.theme();
    display.setTextRole(TextRole::Small);
    display.setTextDatum(Datum::TopLeft);
    display.setTextColor(t.mutedText);
    const String gap = String(Device::bootButton.config().multiClickMs);
    display.drawString("Clicks report " + gap + " ms after the last release.", kMargin, kCardY + kCardH + 10);
    group.draw();
  }

  void update()
  {
    note(power);
    note(boot);
    group.update();
  }
}

const Screen2 ButtonsScreen = [] {
  Screen2 s{};
  s.name = "Buttons";
  s.draw = draw;
  s.update = update;
  s.onEnter = enter;
  return s;
}();
