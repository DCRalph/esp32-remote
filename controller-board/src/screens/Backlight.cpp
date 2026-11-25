// Backlight brightness by PWM, to test dimming on this panel.
#include "screens/Screens.h"

#include "Device.h"
#include "config.h"

namespace
{
  int level = 100;
  Slider slider("Brightness", &level, 5, 100, 5, [](int v)
                { Device::setBrightness(static_cast<uint8_t>(v)); });
  Button backButton("Back", []()
                    { screenManager.back(); });
  FocusGroup group;
  bool built = false;

  void enter()
  {
    level = Device::brightness();
    if (built)
      return;
    built = true;
    slider.suffix = "%";
    slider.bounds = {12, 70, static_cast<int16_t>(LCD_WIDTH - 24), 64};
    backButton.bounds = {static_cast<int16_t>(LCD_WIDTH - 96), static_cast<int16_t>(LCD_HEIGHT - 32), 90, 28};
    group.add(slider);
    group.add(backButton);
  }

  void draw()
  {
    const Theme &t = display.theme();
    display.setTextRole(TextRole::Body);
    display.setTextColor(t.mutedText);
    display.setTextDatum(Datum::TopLeft);
    display.drawString("Drag to set. Resets on restart.", 12, 40);
    group.draw();
  }

  void update()
  {
    group.update();
  }
}

const Screen2 BacklightScreen = [] {
  Screen2 s{};
  s.name = "Backlight";
  s.draw = draw;
  s.update = update;
  s.onEnter = enter;
  return s;
}();
