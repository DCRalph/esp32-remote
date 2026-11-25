// The keyboard on the touch panel types onto the LCD.
#include "screens/Screens.h"

#include "config.h"
#include "Displays.h"

namespace
{
  String typed;
  Keyboard keyboard;
  bool built = false;

  void lcdEnter()
  {
    if (!built)
    {
      built = true;
      keyboard.setBounds({0, 60, TOUCH_WIDTH, TOUCH_HEIGHT - 60});
      keyboard.onKey = [](char c)
      {
        if (typed.length() < 120)
          typed += c;
      };
      keyboard.onDelete = []()
      {
        if (typed.length())
          typed.remove(typed.length() - 1);
      };
      keyboard.onDone = []()
      { screenManager.back(); };
      keyboard.onCancel = keyboard.onDone;
    }
    touchScreens.setScreen(&Demos::TypingTouch);
  }

  void lcdExit()
  {
    touchScreens.setScreen(&TouchHomeScreen);
  }

  void lcdDraw()
  {
    const int16_t top = MenuStyle::contentTopY() + 4;
    display.wrapTextInBounds(typed.length() ? typed + "_" : String("Type on the touch screen"), 6, top,
                             display.width() - 12, display.height() - top - 18, TextRole::Title, 2,
                             typed.length() ? display.theme().text : Color::DarkGrey, Datum::TopLeft);
    drawText(display, String(typed.length()) + " chars, 2 clicks: done", 6, display.height() - 2, TextRole::Small,
             Datum::BottomLeft, Color::DarkGrey);
  }

  void lcdUpdate()
  {
    for (const InputEvent &e : Inputs::events())
    {
      if (isExitEvent(e) || (e.kind == InputKind::Select && e.value == 1))
      {
        screenManager.back();
        return;
      }
    }
  }

  void touchDraw()
  {
    // Last stretch of the text, so the cursor stays in view.
    const String tail = typed.length() > 24 ? typed.substring(typed.length() - 24) : typed;
    drawText(touchDisplay, tail + "_", 8, 32);
    keyboard.draw();
  }
}

const Screen2 Demos::TypingLcd = {
    .name = "Type to LCD",
    .draw = lcdDraw,
    .update = lcdUpdate,
    .onEnter = lcdEnter,
    .onExit = lcdExit,
};

const Screen2 Demos::TypingTouch = {
    .name = "Keyboard",
    .draw = touchDraw,
    .update = []()
    { keyboard.update(); },
    .onEnter = nullptr,
    .onExit = nullptr,
};
