#pragma once

#include "config.h"
#include "Buttons.h"
#include "IO/Display.h"
#include "IO/ScreenManager.h"

class Popup
{
public:
  Popup(String _title, String _message);
  void draw();
  void update();

  String title;
  String message;
};