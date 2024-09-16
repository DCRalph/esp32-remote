#pragma once

#include "config.h"
#include "Display.h"
#include <vector>
#include "Popup.h"

class Screen;
class Popup;

class ScreenManager
{
private:
  int currentScreen;
  std::vector<Screen *> screens;
  std::vector<int> screenHistory;

  Popup *popup;
  bool popupActive;

  void updateHistory(void);

public:
  ScreenManager();

  void init();

  void draw(void);
  void update(void);

  void setScreen(int screen);
  void setScreen(String screenName);

  void addScreen(Screen *screen);

  void removeScreen(int screen);
  void removeScreen(String screenName);

  Screen *getCurrentScreen(void);

  void back(void);
  void clearHistory(void);

  void showPopup(Popup *popup);
  void closePopup(void);
  void drawPopup(void);
};

extern ScreenManager screenManager;