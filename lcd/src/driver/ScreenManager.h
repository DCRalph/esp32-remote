#pragma once

#include <Arduino.h>
#include "driver/Display.h"
#include <vector>

class Screen;

class ScreenManager
{
private:
  int currentScreen;
  std::vector<Screen *> screens;
  std::vector<int> screenHistory;

  void updateHistory(void);

public:
  ScreenManager();

  void init();

  void update(void);
  void draw(void);

  void setScreen(int screen);
  void setScreen(String screenName);

  void addScreen(Screen *screen);

  void removeScreen(int screen);
  void removeScreen(String screenName);

  Screen *getCurrentScreen(void);

  void back(void);
  void clearHistory(void);
};

extern ScreenManager screenManager;