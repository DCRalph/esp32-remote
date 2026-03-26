#include "ScreenManager.h"

#include <esp_log.h>

static const char *TAG = "ScreenManager";

ScreenManager::ScreenManager() = default;

void ScreenManager::init(Display &displayRef)
{
  display = &displayRef;
  displayRef.setScreenManager(this);
#ifdef ENABLE_STUCK_SCREEN_GUARD
  lastButtonCheckTime = 0;
  lastCheckedScreen = nullptr;
  popupShownForCurrentScreen = false;
#endif
  backgroundTaskHandle = nullptr;
  backgroundWatchdogHandle = nullptr;
  backgroundTaskOwner = nullptr;
  backgroundTaskStep = nullptr;
  backgroundTaskShouldStop = false;
  backgroundTaskStepInProgress = false;
  backgroundTaskStepStartMs = 0;
  ESP_LOGI(TAG, "Initialized");
}

void ScreenManager::draw(void)
{
  const Screen2 *s = currentScreen;
  if (s && s->draw)
    s->draw();
}

void ScreenManager::update(void)
{
  if (popupActive)
    return;

  const Screen2 *s = currentScreen;
  if (s && s->update)
    s->update();

  applyPendingScreenChange();

  checkForNoButtons();
}

const Screen2 *ScreenManager::getCurrentScreen(void)
{
  if (pendingScreen)
    return pendingScreen;
  return currentScreen;
}

bool ScreenManager::applyPendingScreenChange(void)
{
  if (pendingScreen == nullptr)
    return false;

    Serial.printf("Applying pending screen change to: %s\n", pendingScreen->name);

  if (pendingScreen->draw == nullptr || pendingScreen->update == nullptr)
  {
    showPopup(new Popup("Error", "Pending screen has no draw or update function"));
    pendingScreen = nullptr;
    return false;
  }

  if (currentScreen == pendingScreen)
  {
    ESP_LOGI(TAG, "Same screen, skipping");
    pendingScreen = nullptr;
    return false;
  }

  stopBackgroundTask();

  if (currentScreen && currentScreen->onExit)
    currentScreen->onExit();

  currentScreen = pendingScreen;
  pendingScreen = nullptr;

  ESP_LOGI(TAG, ">> %s", currentScreen->name);

  updateHistory(currentScreen);

  if (currentScreen && currentScreen->onEnter)
    currentScreen->onEnter();

#ifdef ENABLE_STUCK_SCREEN_GUARD
  lastButtonCheckTime = millis();
  lastCheckedScreen = currentScreen;
  popupShownForCurrentScreen = false;
#endif

  return true;
}

void ScreenManager::setScreen(const Screen2 *screen)
{
  Serial.printf("Setting screen to: %s\n", screen->name);
  pendingScreen = screen;
}

void ScreenManager::back(void)
{
  if (screenHistory.size() > 1)
  {
    screenHistory.pop_back();
    pendingScreen = screenHistory.back();
    ESP_LOGI(TAG, "Going back to: %s", pendingScreen->name);
  }
  else
  {
    ESP_LOGI(TAG, "Cannot go back - no screen history");
  }
}

void ScreenManager::clearHistory(void)
{
  const Screen2 *current = nullptr;
  if (!screenHistory.empty())
    current = screenHistory.back();

  screenHistory.clear();

  if (current != nullptr)
    screenHistory.push_back(current);
}

void ScreenManager::updateHistory(const Screen2 *screen)
{
  if (screen == nullptr)
  {
    ESP_LOGW(TAG, "Attempted to add null screen to history");
    return;
  }

  if (screenHistory.empty() || screenHistory.back() != screen)
  {
    screenHistory.push_back(screen);
    if (screenHistory.size() > 10)
      screenHistory.erase(screenHistory.begin());
  }
}

bool ScreenManager::goToHistoryIndex(size_t index)
{
  if (index < screenHistory.size())
  {
    ESP_LOGI(TAG, "Navigating to history index %u: %s", (unsigned)index, screenHistory[index]->name);

    if (index != screenHistory.size() - 1)
    {
      pendingScreen = screenHistory[index];
      screenHistory.resize(index + 1);
      return true;
    }

    ESP_LOGI(TAG, "Already at requested screen");
    return false;
  }

  ESP_LOGW(TAG, "Invalid history index: %u", (unsigned)index);
  return false;
}

uint16_t ScreenManager::getScreenTopBarColor(void)
{
  const Screen2 *s = getCurrentScreen();
  if (!s)
    return TFT_BLUE;
  return s->topBarColor;
}

uint16_t ScreenManager::getScreenTopBarTextColor(void)
{
  const Screen2 *s = getCurrentScreen();
  if (!s)
    return TFT_BLACK;
  return s->topBarTextColor;
}

void ScreenManager::showPopup(Popup *p)
{
  popup = p;
  popupActive = true;
}

void ScreenManager::closePopup(void)
{
  popupActive = false;
}

void ScreenManager::drawPopup(void)
{
  if (popupActive && popup)
  {
    popup->draw();
    popup->update();
  }
}

bool ScreenManager::isPopupActive(void)
{
  return popupActive;
}

void ScreenManager::checkForNoButtons()
{
#ifndef ENABLE_STUCK_SCREEN_GUARD
  return;
#else
  if (!currentScreen)
    return;

  if (exceptionPredicate && exceptionPredicate(currentScreen))
    return;

  if (popupShownForCurrentScreen)
    return;

  if (popupActive)
    return;

  unsigned long t = millis();
  if (t - lastButtonCheckTime < BUTTON_CHECK_INTERVAL_MS)
    return;

  lastButtonCheckTime = t;
  lastCheckedScreen = currentScreen;

  // Placeholder: full parity requires Menu/active-control registration.
  (void)lastCheckedScreen;
#endif
}

#ifdef ENABLE_STUCK_SCREEN_GUARD
void ScreenManager::resetPopupShownFlag(void)
{
  popupShownForCurrentScreen = false;
  lastButtonCheckTime = millis();
}
#endif

void ScreenManager::backgroundTaskRunner(void *pvParameters)
{
  ScreenManager *manager = static_cast<ScreenManager *>(pvParameters);
  if (manager == nullptr)
  {
    vTaskDelete(nullptr);
    return;
  }

  const Screen2 *ownerScreen = manager->backgroundTaskOwner;
  while (manager->isBackgroundTaskActive(ownerScreen))
  {
    if (manager->backgroundTaskStep == nullptr)
      break;

    manager->backgroundTaskStepInProgress = true;
    manager->backgroundTaskStepStartMs = millis();
    manager->backgroundTaskStep(ownerScreen);
    manager->backgroundTaskStepInProgress = false;

    const uint32_t waitStepMs = 50;
    uint32_t waitedMs = 0;
    while (waitedMs < manager->backgroundTaskLoopDelayMs && manager->isBackgroundTaskActive(ownerScreen))
    {
      vTaskDelay(pdMS_TO_TICKS(waitStepMs));
      waitedMs += waitStepMs;
    }
  }

  manager->backgroundTaskStepInProgress = false;
  manager->backgroundTaskHandle = nullptr;
  manager->backgroundTaskOwner = nullptr;
  manager->backgroundTaskStep = nullptr;
  manager->backgroundTaskShouldStop = false;
  manager->backgroundTaskStepStartMs = 0;

  if (manager->backgroundWatchdogHandle != nullptr)
  {
    vTaskDelete(manager->backgroundWatchdogHandle);
    manager->backgroundWatchdogHandle = nullptr;
  }

  vTaskDelete(nullptr);
}

void ScreenManager::backgroundTaskWatchdogRunner(void *pvParameters)
{
  ScreenManager *manager = static_cast<ScreenManager *>(pvParameters);
  if (manager == nullptr)
  {
    vTaskDelete(nullptr);
    return;
  }

  while (manager->backgroundTaskHandle != nullptr)
  {
    if (manager->backgroundTaskStepInProgress)
    {
      uint32_t elapsedMs = millis() - manager->backgroundTaskStepStartMs;
      if (elapsedMs > manager->backgroundTaskWatchdogTimeoutMs)
      {
        ESP_LOGE(TAG, "Background task watchdog timeout (%ums). Killing task.", (unsigned)elapsedMs);
        TaskHandle_t taskToKill = manager->backgroundTaskHandle;

        manager->backgroundTaskShouldStop = true;
        manager->backgroundTaskStepInProgress = false;
        manager->backgroundTaskStepStartMs = 0;
        manager->backgroundTaskHandle = nullptr;
        manager->backgroundTaskOwner = nullptr;
        manager->backgroundTaskStep = nullptr;

        if (taskToKill != nullptr)
          vTaskDelete(taskToKill);

        break;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }

  manager->backgroundWatchdogHandle = nullptr;
  vTaskDelete(nullptr);
}

bool ScreenManager::startBackgroundTask(ScreenBackgroundStep stepCode,
                                        const char *name,
                                        uint32_t stackDepth,
                                        uint32_t loopDelayMs,
                                        uint32_t watchdogTimeoutMs)
{
  if (backgroundTaskHandle != nullptr || currentScreen == nullptr || stepCode == nullptr)
    return false;

  backgroundTaskShouldStop = false;
  backgroundTaskOwner = currentScreen;
  backgroundTaskStep = stepCode;
  backgroundTaskLoopDelayMs = loopDelayMs;
  backgroundTaskWatchdogTimeoutMs = watchdogTimeoutMs;
  backgroundTaskStepInProgress = false;
  backgroundTaskStepStartMs = 0;

  BaseType_t created = xTaskCreate(ScreenManager::backgroundTaskRunner, name, stackDepth, this, 1, &backgroundTaskHandle);
  if (created != pdPASS)
  {
    backgroundTaskOwner = nullptr;
    backgroundTaskStep = nullptr;
    backgroundTaskHandle = nullptr;
    backgroundTaskShouldStop = false;
    return false;
  }

  BaseType_t watchdogCreated = xTaskCreate(ScreenManager::backgroundTaskWatchdogRunner, "screenBgWdog", 3072, this, 2, &backgroundWatchdogHandle);
  if (watchdogCreated != pdPASS)
  {
    TaskHandle_t taskToKill = backgroundTaskHandle;
    backgroundTaskHandle = nullptr;
    backgroundTaskOwner = nullptr;
    backgroundTaskStep = nullptr;
    backgroundTaskShouldStop = true;
    if (taskToKill != nullptr)
      vTaskDelete(taskToKill);
    return false;
  }

  return true;
}

void ScreenManager::stopBackgroundTask(void)
{
  backgroundTaskShouldStop = true;
}

bool ScreenManager::isBackgroundTaskActive(const Screen2 *screen)
{
  return (backgroundTaskHandle != nullptr) && (backgroundTaskOwner == screen) && !backgroundTaskShouldStop;
}

ScreenManager screenManager;
