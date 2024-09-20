#include "ScreenManager.h"

static const char* TAG = "ScreenManager";

ScreenManager::ScreenManager()
{
  currentScreen = -1;
  popupActive = false;
}

void ScreenManager::init()
{
  // Serial.println("\t[INFO] [SCREEN MANAGER] Initialized");
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
    // Serial.println("[WARN] [SCREEN MANAGER] Screen already set");
    ESP_LOGW(TAG, "Screen already set");
    return;
  }

  if (currentScreen != -1)
  {
    screens[currentScreen]->onExit();
    
  }

  currentScreen = screen;

  // Serial.println("[INFO] [SCREEN MANAGER] >> " + screens[screen]->name);
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
  // Serial.println("[WARN] [SCREEN MANAGER] Screen not found");
  ESP_LOGW(TAG, "Screen not found");
}

void ScreenManager::addScreen(Screen *screen)
{
  screens.push_back(screen);

  // String msg = "\t[INFO] [SCREEN MANAGER] " + screen->name + " added";
  // Serial.println(msg);
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
  // Serial.println("[WARN] [SCREEN MANAGER] Screen not found");
  ESP_LOGW(TAG, "Screen not found");
}

Screen *ScreenManager::getCurrentScreen(void)
{
  return screens[currentScreen];
}

void ScreenManager::back(void)
{
  if (screenHistory.size() > 1)
  {
    screenHistory.pop_back();

    screens[currentScreen]->onExit();

    currentScreen = screenHistory.back();

    // Serial.println("[INFO] [SCREEN MANAGER] << " + screens[currentScreen]->name);
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

  // for (int i = 0; i < screenHistory.size(); i++)
  // {
  //   Serial.print(screenHistory[i]);
  //   Serial.print(" ");
  // }
  // Serial.print(" ");

  // for (int i = 0; i < screenHistory.size(); i++)
  // {
  //   if (screenHistory[i] == currentScreen)
  //   {
  //     screenHistory.erase(screenHistory.begin() + i);
  //   }
  // }

  // for (int i = 0; i < screenHistory.size(); i++)
  // {
  //   Serial.print(screenHistory[i]);
  //   Serial.print(" ");
  // }
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

void ScreenManager::drawPopup(void){
  if(popupActive){
    this->popup->draw();
    this->popup->update();
  }
}

ScreenManager screenManager;
