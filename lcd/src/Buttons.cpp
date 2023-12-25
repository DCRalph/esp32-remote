#include "Buttons.h"

ClickButton ClickButtonUP(14, LOW);
ClickButton ClickButtonDOWN(0, LOW);

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