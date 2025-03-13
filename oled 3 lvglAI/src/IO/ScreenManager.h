#pragma once

#include "config.h"
#include <lvgl.h>
#include <vector>
#include <string>
#include "Popup.h"

class LVScreen;
class Popup;

class ScreenManager
{
private:
  std::vector<lv_obj_t *> screenHistory;
  lv_group_t *navGroup; // Navigation group for input handling
  Popup *currentPopup;
  bool popupActive;

  void setInitialFocus(lv_obj_t *screen); // New private method

public:
  ScreenManager();

  void init();
  void setNavGroup(lv_group_t *group) { navGroup = group; }

  // Screen management
  void pushScreen(lv_obj_t *screen);
  void popScreen();
  void setScreen(lv_obj_t *screen);
  void setScreen(const std::string &name);
  lv_obj_t *getCurrentScreen();
  void clearHistory();

  // Popup management
  void showPopup(Popup *popup);
  void closePopup();
  bool isPopupActive() const { return popupActive; }
  void updatePopup();

  // Input handling
  void handleUpButton();
  void handleDownButton();
  void handleTriggerButton();
};

extern ScreenManager screenManager;