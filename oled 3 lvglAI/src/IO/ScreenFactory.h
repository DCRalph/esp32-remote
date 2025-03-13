#pragma once

#include "config.h"
#include "LVScreen.h"
#include "../screens/Home.h"
#include "../screens/Settings/batteryScreen.h"
#include "../screens/SettingsScreen.h"
#include "../screens/Error.h"
#include "../screens/Control.h"
#include "../screens/RemoteRelay.h"
#include "../screens/boxThingEncoder.h"
#include "../screens/Send.h"
#include "../screens/Control/Car.h"
#include "../screens/Control/CarFlash.h"
#include "../screens/Control/CarLocks.h"
#include "../screens/Settings/RSSIMeter.h"
#include "../screens/Settings/SystemInfoScreen.h"
#include "../screens/Settings/WiFiInfo.h"
#include <memory>
#include <unordered_map>

class ScreenFactory
{
private:
  lv_group_t *navGroup;
  std::unordered_map<std::string, std::unique_ptr<LVScreen>> screens;

public:
  ScreenFactory(lv_group_t *group = nullptr) : navGroup(group) {}

  void setNavGroup(lv_group_t *group)
  {
    navGroup = group;
    // Update nav group for existing screens
    for (auto &pair : screens)
    {
      pair.second->setNavGroup(group);
    }
  }

  lv_obj_t *getScreen(const std::string &name)
  {
    auto it = screens.find(name);
    if (it != screens.end())
    {
      return it->second->getScreen();
    }

    // Create new screen if it doesn't exist
    std::unique_ptr<LVScreen> screen;

    if (name == "Home")
    {
      screen = std::unique_ptr<HomeScreen>(new HomeScreen(name, navGroup));
    }
    else if (name == "Battery")
    {
      screen = std::unique_ptr<BatteryScreen>(new BatteryScreen(name, navGroup));
    }
    else if (name == "Settings")
    {
      screen = std::unique_ptr<Settings>(new Settings(name, navGroup));
    }
    else if (name == "Error")
    {
      screen = std::unique_ptr<ErrorScreen>(new ErrorScreen(name, navGroup));
    }
    else if (name == "Control")
    {
      screen = std::unique_ptr<ControlScreen>(new ControlScreen(name, navGroup));
    }
    else if (name == "RemoteRelay")
    {
      screen = std::unique_ptr<RemoteRelayScreen>(new RemoteRelayScreen(name, navGroup));
    }
    else if (name == "BoxThingEncoder")
    {
      screen = std::unique_ptr<BoxThingEncoderScreen>(new BoxThingEncoderScreen(name, navGroup));
    }
    else if (name == "Send")
    {
      screen = std::unique_ptr<SendScreen>(new SendScreen(name, navGroup));
    }
    else if (name == "Car")
    {
      screen = std::unique_ptr<CarControlScreen>(new CarControlScreen(name, navGroup));
    }
    else if (name == "CarFlash")
    {
      screen = std::unique_ptr<CarFlashScreen>(new CarFlashScreen(name, navGroup));
    }
    else if (name == "CarLocks")
    {
      screen = std::unique_ptr<CarLocksScreen>(new CarLocksScreen(name, navGroup));
    }
    else if (name == "RSSIMeter")
    {
      screen = std::unique_ptr<RSSIMeter>(new RSSIMeter(name, navGroup));
    }
    else if (name == "SystemInfo")
    {
      screen = std::unique_ptr<SystemInfoScreen>(new SystemInfoScreen(name, navGroup));
    }
    else if (name == "WiFiInfo")
    {
      screen = std::unique_ptr<WiFiInfoScreen>(new WiFiInfoScreen(name, navGroup));
    }
    else
    {
      // Default to error screen if screen not found
      screen = std::unique_ptr<ErrorScreen>(new ErrorScreen("Error", navGroup));
    }

    lv_obj_t *screenObj = screen->getScreen();
    screens[name] = std::move(screen);
    return screenObj;
  }

  void removeScreen(const std::string &name)
  {
    screens.erase(name);
  }

  void clearScreens()
  {
    screens.clear();
  }
};

extern ScreenFactory screenFactory;