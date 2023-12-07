#include "driver/ScreenManager.h"

// void ScreenManager::init(Display *_display)
// {
//   display = _display;

//   // logger.println("\tScreen Manager initialized");
// }

void ScreenManager::update(void)
{
  screens[currentScreen]->update();
}

void ScreenManager::draw(void)
{
  screens[currentScreen]->draw();
}

String ScreenManager::getCurrentScreenName(void)
{
  return screens[currentScreen]->name;
}

void ScreenManager::addScreen(Screen *screen)
{
  screens.push_back(screen);

  String msg = "\tScreen " + screen->name + " added";
}

void ScreenManager::setScreen(int screenIdx)
{

  if (screenIdx < 0 || screenIdx >= screens.size())
  {
    currentScreen = -1;
    return;
  }

  if (screens[screenIdx]->id == "")
  {
    currentScreen = -1;
    return;
  }

  currentScreen = screenIdx;
}

void ScreenManager::setScreen(String screenId)
{
  for (int i = 0; i < screens.size(); i++)
  {
    if (screens[i]->id == screenId)
    {
      currentScreen = i;
      return;
    }
  }

  currentScreen = -1;
}

void ScreenManager::removeScreen(int screenIdx)
{
  screens.erase(screens.begin() + screenIdx);
}

void ScreenManager::removeScreen(String screenId)
{
  for (int i = 0; i < screens.size(); i++)
  {
    if (screens[i]->id == screenId)
    {
      screens.erase(screens.begin() + i);
      return;
    }
  }
}

ScreenManager screenManager;