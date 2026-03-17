#pragma once

#include "config.h"
#include "IO/Buttons.h"
#include "Display.cpp"
#include "ScreenManager.h"

class Popup
{
public:
  Popup(String _title, String _message);
  virtual void draw();
  virtual void update();

  String title;
  String message;
};

class AutoClosePopup : public Popup
{
private:
  unsigned long startTime;
  unsigned long duration;

  int progressBarWidth = 0;

public:
  AutoClosePopup(String _title, String _message, unsigned long _duration);
  void draw() override;
  void update() override;
};
