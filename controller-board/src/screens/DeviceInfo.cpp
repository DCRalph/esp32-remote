// What this unit reported at boot, and how the display is doing.
#include "screens/Screens.h"

#include "Device.h"
#include "Touch.h"
#include "config.h"

namespace
{
  Button backButton("Back", []()
                    { screenManager.back(); });
  FocusGroup group;
  bool built = false;

  void line(int16_t y, const char *label, const String &value)
  {
    display.setTextRole(TextRole::Small);
    display.setTextDatum(Datum::TopLeft);
    const Theme &t = display.theme();
    display.setTextColor(t.mutedText);
    display.drawString(label, 12, y);
    display.setTextColor(t.text);
    display.drawString(value, 130, y);
  }

  void enter()
  {
    if (built)
      return;
    built = true;
    backButton.bounds = {static_cast<int16_t>(LCD_WIDTH - 96), static_cast<int16_t>(LCD_HEIGHT - 32), 90, 28};
    group.add(backButton);
  }

  void draw()
  {
    const Device::Info &info = Device::info();
    const FrameStats &stats = display.stats();
    int16_t y = 34;
    const int16_t step = 18;
    line(y, "Serial", info.found ? String(info.serial) : String("not provisioned"));
    line(y += step, "Hardware", "v" + String(info.hardwareVersion));
    line(y += step, "Device type", String(info.deviceType));
    line(y += step, "Rotation", String(info.rotation));
    line(y += step, "Backlight on", info.backlightActiveHigh ? "high" : "low");
    const Touch::Source cal = Touch::source();
    line(y += step, "Touch cal", cal == Touch::Source::Saved ? "saved on this board" : cal == Touch::Source::Bioreactor ? "from bioreactor" : "generic");
    line(y += step, "Frame", String(stats.fps) + " fps, " + String(stats.avgPushedPercent) + "% pushed");
    line(y += step, "Heap / PSRAM", String(ESP.getFreeHeap() / 1024) + "K / " + String(ESP.getFreePsram() / 1024) + "K");
    group.draw();
  }

  void update()
  {
    group.update();
  }
}

const Screen2 DeviceScreen = [] {
  Screen2 s{};
  s.name = "Device";
  s.draw = draw;
  s.update = update;
  s.onEnter = enter;
  s.redrawMs = 1000; // stats move once a second
  return s;
}();
