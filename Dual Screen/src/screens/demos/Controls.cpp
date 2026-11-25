// One set of values, two ways to change it: a menu on the LCD and a lamp
// preview with a slider and colour buttons on the touch panel.
#include "screens/Screens.h"

#include "config.h"
#include "Displays.h"

namespace
{
  bool gOn = true;
  int gLevel = 60;

  constexpr uint16_t kColours[] = {Color::rgb(255, 180, 90), Color::rgb(170, 210, 255), Color::Red, Color::Green,
                                   Color::Blue};
  const std::vector<String> kColourNames = {"Warm", "Cool", "Red", "Green", "Blue"};

  Menu menu(MenuSize::Medium);
  MenuItemBack backItem;
  MenuItemToggle onItem("Light", &gOn);
  MenuItemNumber<int> levelItem("Level %", &gLevel, 0, 100, 5);
  MenuItemSelect colourItem("Colour", kColourNames);

  Button colourButtons[] = {Button("Warm"), Button("Cool"), Button("Red"), Button("Green"), Button("Blue")};
  Button onButton("On/Off", []()
                  { gOn = !gOn; });
  Button backButton("Back", []()
                    { screenManager.back(); });
  FocusGroup group;
  Rect lamp, slider;
  bool built = false;

  uint16_t scaled(uint16_t c, int percent)
  {
    const uint8_t r = ((c >> 11) & 0x1F) * 255 / 31, g = ((c >> 5) & 0x3F) * 255 / 63, b = (c & 0x1F) * 255 / 31;
    return Color::rgb(r * percent / 100, g * percent / 100, b * percent / 100);
  }

  void build()
  {
    built = true;
    for (MenuItem *item : {static_cast<MenuItem *>(&backItem), static_cast<MenuItem *>(&onItem),
                           static_cast<MenuItem *>(&levelItem), static_cast<MenuItem *>(&colourItem)})
      menu.addMenuItem(item);
    levelItem.setFastUpdate(true);

    for (int i = 0; i < 5; i++)
    {
      colourButtons[i].onPress = [i]()
      { colourItem.setCurrentIndex(i); };
      group.add(colourButtons[i]);
    }
    group.add(onButton);
    group.add(backButton);
    group.setColumns(2);

    // Touch panel layout: lamp on the left, buttons on the right, slider along the bottom.
    const int16_t top = 30, W = TOUCH_WIDTH, H = TOUCH_HEIGHT;
    lamp = {10, top, 130, 140};
    slider = {10, static_cast<int16_t>(H - 44), static_cast<int16_t>(W - 20), 34};
    const int16_t bx = 150, bw = (W - bx - 10 - 6) / 2, bh = 30;
    for (int i = 0; i < 7; i++)
    {
      Button &b = i < 5 ? colourButtons[i] : (i == 5 ? onButton : backButton);
      b.bounds = {static_cast<int16_t>(bx + (i % 2) * (bw + 6)), static_cast<int16_t>(top + (i / 2) * (bh + 6)), bw,
                  bh};
    }
  }

  void lcdEnter()
  {
    if (!built)
      build();
    touchScreens.setScreen(&Demos::ControlsTouch);
  }

  void lcdExit()
  {
    touchScreens.setScreen(&TouchHomeScreen);
  }

  void touchDraw()
  {
    Display &d = touchDisplay;
    const uint16_t colour = kColours[colourItem.getCurrentIndex()];
    if (gOn)
      d.fillRoundRect(lamp.x, lamp.y, lamp.w, lamp.h, 12, scaled(colour, max(gLevel, 5)));
    d.drawRoundRect(lamp.x, lamp.y, lamp.w, lamp.h, 12, d.theme().text);
    drawText(d, gOn ? String(gLevel) + "%" : String("OFF"), lamp.x + lamp.w / 2, lamp.y + lamp.h / 2,
             TextRole::Title, Datum::MiddleCenter, gOn && gLevel > 50 ? Color::Black : Color::White);

    d.drawRect(slider.x, slider.y, slider.w, slider.h, d.theme().text);
    d.fillRect(slider.x + 2, slider.y + 2, (slider.w - 4) * gLevel / 100, slider.h - 4, d.theme().accent);
    drawText(d, "Drag to set level", slider.x, slider.y - 4, TextRole::Small, Datum::BottomLeft,
             d.theme().mutedText);

    group.draw();
  }

  void touchUpdate()
  {
    for (const InputEvent &e : Inputs::events())
    {
      if ((e.kind == InputKind::PointerDown || e.kind == InputKind::PointerMove) && slider.contains(e.x, e.y))
        gLevel = constrain((e.x - slider.x) * 100 / slider.w, 0, 100);
    }
    group.update();
  }
}

const Screen2 Demos::ControlsLcd = {
    .name = "Shared controls",
    .draw = []()
    { menu.draw(); },
    .update = []()
    { menu.update(); },
    .onEnter = lcdEnter,
    .onExit = lcdExit,
};

const Screen2 Demos::ControlsTouch = {
    .name = "Lamp",
    .draw = touchDraw,
    .update = touchUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
