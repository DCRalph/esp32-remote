// Each screen asks the other: a popup raised on one display is answered with
// the other display's input.
#include "screens/Screens.h"

#include "config.h"
#include "Displays.h"

namespace
{
  String touchAnswer = "-", lcdAnswer = "-";

  String answer(int choice)
  {
    return choice < 0 ? String("cancelled") : (choice == 1 ? String("Yes") : String("No"));
  }

  Menu menu(MenuSize::Medium);
  MenuItemBack backItem;
  MenuItemAction askTouchItem("Ask touch screen", 1, []()
                              {
                                touchAnswer = "waiting";
                                touchScreens.showPopup(new ChoicePopup("From the LCD", "Tap an answer here.", {"No", "Yes"},
                                                                       [](int c)
                                                                       { touchAnswer = answer(c); })); });
  MenuItemAction toastTouchItem("Toast touch", 1, []()
                                { touchScreens.showPopup(new AutoClosePopup("Hello", "Sent from the LCD", 2000)); });
  MenuItemString touchAnswerItem("Touch said", &touchAnswer);

  Button askLcdButton("Ask LCD", []()
                      {
                        lcdAnswer = "waiting";
                        screenManager.showPopup(new ChoicePopup("From touch", "Up/down, double click.", {"No", "Yes"},
                                                                [](int c)
                                                                { lcdAnswer = answer(c); })); });
  Button toastLcdButton("Toast LCD", []()
                        { screenManager.showPopup(new AutoClosePopup("Hello", "Sent from the touch screen", 2000)); });
  Button backButton("Back", []()
                    { screenManager.back(); });
  FocusGroup group;
  bool built = false;

  void lcdEnter()
  {
    if (!built)
    {
      built = true;
      for (MenuItem *item : {static_cast<MenuItem *>(&backItem), static_cast<MenuItem *>(&askTouchItem),
                             static_cast<MenuItem *>(&toastTouchItem), static_cast<MenuItem *>(&touchAnswerItem)})
        menu.addMenuItem(item);
      int16_t y = 40;
      for (Button *b : {&askLcdButton, &toastLcdButton, &backButton})
      {
        b->bounds = {20, y, TOUCH_WIDTH - 40, 40};
        group.add(*b);
        y += 50;
      }
    }
    touchScreens.setScreen(&Demos::CrossoverTouch);
  }

  void lcdExit()
  {
    touchScreens.setScreen(&TouchHomeScreen);
  }

  void touchDraw()
  {
    group.draw();
    drawText(touchDisplay, "LCD said: " + lcdAnswer, 20, TOUCH_HEIGHT - 26);
  }
}

const Screen2 Demos::CrossoverLcd = {
    .name = "Cross popups",
    .draw = []()
    { menu.draw(); },
    .update = []()
    { menu.update(); },
    .onEnter = lcdEnter,
    .onExit = lcdExit,
};

const Screen2 Demos::CrossoverTouch = {
    .name = "Cross popups",
    .draw = touchDraw,
    .update = []()
    { group.update(); },
    .onEnter = nullptr,
    .onExit = nullptr,
};
