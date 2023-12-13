#pragma once

#include <Arduino.h>
#include "driver/Display.h"
#include <vector>

class Display;
class Screen;

class ScreenManager
{
public:
  // Display *display;
  int currentScreen = 0;
  std::vector<Screen *> screens;

  // void init(Display *_display);

  void update(void);
  void draw(void);

  String getCurrentScreenName(void);
  String getNameFromId(String id);

  void addScreen(Screen *screen);

  void setScreen(int screenIdx);
  void setScreen(String screenId);

  void removeScreen(int screenIdx);
  void removeScreen(String screenId);
};

extern ScreenManager screenManager;