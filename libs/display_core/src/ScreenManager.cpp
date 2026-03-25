#include "ScreenManager.h"

static const char *TAG = "ScreenManager";

ScreenManager::ScreenManager() : display(nullptr)
{
  currentScreen = -1;
  popupActive = false;
}

void ScreenManager::init(Display &displayRef)
{
  display = &displayRef;
  displayRef.setScreenManager(this);
  ESP_LOGI(TAG, "Initialized");
}

void ScreenManager::draw(void)
{
  screens[currentScreen]->draw();
}

void ScreenManager::update(void)
{
  if (!popupActive)
    screens[currentScreen]->update();
}

void ScreenManager::setScreen(int screen)
{
  if (screen == currentScreen)
  {
    ESP_LOGW(TAG, "Screen already set");
    return;
  }

  if (currentScreen != -1)
  {
    screens[currentScreen]->onExit();
  }

  currentScreen = screen;

  ESP_LOGI(TAG, ">> %s", screens[screen]->name.c_str());

  updateHistory();
  screens[screen]->onEnter();
}

void ScreenManager::setScreen(String screenName)
{
  for (std::vector<Screen *>::size_type i = 0; i < screens.size(); i++)
  {
    if (screens[i]->name == screenName)
    {
      setScreen(i);
      return;
    }
  }
  ESP_LOGW(TAG, "Screen not found");
}

void ScreenManager::addScreen(Screen *screen)
{
  screens.push_back(screen);
  ESP_LOGI(TAG, "Added: %s", screen->name.c_str());
}

void ScreenManager::removeScreen(int screen)
{
  screens.erase(screens.begin() + screen);
}

void ScreenManager::removeScreen(String screenName)
{
  for (std::vector<Screen *>::size_type i = 0; i < screens.size(); i++)
  {
    if (screens[i]->name == screenName)
    {
      screens.erase(screens.begin() + i);
      return;
    }
  }
  ESP_LOGW(TAG, "Screen not found");
}

Screen *ScreenManager::getCurrentScreen(void)
{
  return screens[currentScreen];
}

uint16_t ScreenManager::getScreenTopBarColor(void)
{
  return screens[currentScreen]->topBarColor;
}

uint16_t ScreenManager::getScreenTopBarTextColor(void)
{
  return screens[currentScreen]->topBarTextColor;
}

void ScreenManager::back(void)
{
  if (screenHistory.size() > 1)
  {
    screenHistory.pop_back();

    screens[currentScreen]->onExit();
    currentScreen = screenHistory.back();

    ESP_LOGI(TAG, "<< %s", screens[currentScreen]->name.c_str());
    screens[currentScreen]->onEnter();
  }
}

void ScreenManager::clearHistory(void)
{
  screenHistory.clear();
}

void ScreenManager::updateHistory(void)
{
  screenHistory.push_back(currentScreen);

  if (screenHistory.size() > 10)
  {
    screenHistory.erase(screenHistory.begin());
  }
}

void ScreenManager::showPopup(Popup *popup)
{
  this->popup = popup;
  popupActive = true;
}

void ScreenManager::closePopup(void)
{
  popupActive = false;
}

void ScreenManager::drawPopup(void)
{
  if (popupActive)
  {
    this->popup->draw();
    this->popup->update();
  }
}

bool ScreenManager::isPopupActive(void)
{
  return popupActive;
}

ScreenManager screenManager;
