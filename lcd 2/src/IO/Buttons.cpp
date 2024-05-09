#include "Buttons.h"

ClickButton ClickButtonUP(BTN_UP_PIN, LOW);
ClickButton ClickButtonDOWN(BTN_DOWN_PIN, LOW);

void Buttons::setup()
{
  ClickButtonUP.debounceTime = debounceTime;
  ClickButtonUP.multiclickTime = multiclickTime;
  ClickButtonUP.longClickTime = longClickTime;

  ClickButtonDOWN.debounceTime = debounceTime;
  ClickButtonDOWN.multiclickTime = multiclickTime;
  ClickButtonDOWN.longClickTime = longClickTime;
}

void Buttons::update()
{
  ClickButtonUP.Update();
  ClickButtonDOWN.Update();
}

Buttons buttons;