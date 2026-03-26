#pragma once

#include "Display.h"
#include "Popup.h"
#include "ScreenTypes.h"
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/** Optional: return true to skip stuck-screen checks for this screen (e.g. Home). */
using ScreenExceptionPredicate = bool (*)(const Screen2 *screen);

class ScreenManager
{
private:
  Display *display = nullptr;

  const Screen2 *currentScreen = nullptr;
  const Screen2 *pendingScreen = nullptr;
  std::vector<const Screen2 *> screenHistory;

  Popup *popup = nullptr;
  bool popupActive = false;

  ScreenExceptionPredicate exceptionPredicate = nullptr;

#ifdef ENABLE_STUCK_SCREEN_GUARD
  unsigned long lastButtonCheckTime = 0;
  const Screen2 *lastCheckedScreen = nullptr;
  bool popupShownForCurrentScreen = false;
  static const unsigned long BUTTON_CHECK_INTERVAL_MS = 10000;
#endif

  TaskHandle_t backgroundTaskHandle = nullptr;
  TaskHandle_t backgroundWatchdogHandle = nullptr;
  const Screen2 *backgroundTaskOwner = nullptr;
  ScreenBackgroundStep backgroundTaskStep = nullptr;
  uint32_t backgroundTaskLoopDelayMs = 1000;
  uint32_t backgroundTaskWatchdogTimeoutMs = 15000;
  volatile bool backgroundTaskShouldStop = false;
  volatile bool backgroundTaskStepInProgress = false;
  volatile uint32_t backgroundTaskStepStartMs = 0;

  void updateHistory(const Screen2 *screen);
  void checkForNoButtons();
  static void backgroundTaskRunner(void *pvParameters);
  static void backgroundTaskWatchdogRunner(void *pvParameters);

public:
  ScreenManager();

  void init(Display &displayRef);

  void draw(void);
  void update(void);

  const Screen2 *getCurrentScreen(void);

  void setScreen(const Screen2 *screen);
  bool applyPendingScreenChange(void);

  void back(void);
  void clearHistory(void);
  bool goToHistoryIndex(size_t index);

  void setExceptionScreenPredicate(ScreenExceptionPredicate fn) { exceptionPredicate = fn; }

#ifdef ENABLE_STUCK_SCREEN_GUARD
  void resetPopupShownFlag(void);
#endif

  uint16_t getScreenTopBarColor(void);
  uint16_t getScreenTopBarTextColor(void);

  void showPopup(Popup *p);
  void closePopup(void);
  void drawPopup(void);
  bool isPopupActive(void);

  bool startBackgroundTask(ScreenBackgroundStep stepCode,
                           const char *name,
                           uint32_t stackDepth,
                           uint32_t loopDelayMs = 1000,
                           uint32_t watchdogTimeoutMs = 15000);
  void stopBackgroundTask(void);
  bool isBackgroundTaskActive(const Screen2 *screen);
};

extern ScreenManager screenManager;
