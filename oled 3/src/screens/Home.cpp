#include "Home.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Popup.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"
#include "IO/GPIO.h"

#include "Control.h"
#include "SettingsScreen.h"
#include "RemoteRelay.h"
#include "boxThingEncoder.h"
#include "TestLoading.h"
#include "Engine.h"

namespace HomeScreenNamespace
{

  namespace
  {
    static Menu menu(MenuSize::Large);

    static MenuItemNavigate controlMenuItem{"Control", &ControlScreen};
    static MenuItemNavigate settingsMenuItem{"Settings", &SettingsScreen};
    static MenuItemAction powerOffMenuItem{"Power Off", 2, []()
                                           { esp_deep_sleep_start(); }};

    static MenuItemAction testPopupItem{"Test Popup", 2, []()
                                        { screenManager.showPopup(new Popup("Test Popup", "This is a test gyY9 popup. pls wrap words TTTTT")); }};

    static MenuItemAction testBlinkItem{"Test Blink", 2, []()
                                        {
                                          uint32_t color = (0 << 16) | (255 << 8) | 0;
                                          btnLed.Blink(color, 200, 3);
                                        }};

    static MenuItemNavigate remoteRelay{"Remote Relay", &RemoteRelayScreen};

    static MenuItemNavigate boxThingEncoderItem{"BoxThingEncoder", &BoxThingEncoderScreen};
    static MenuItemNavigate testLoadingItem{"Test Loading", &TestLoadingScreen};
    static MenuItemNavigate engineItem{"Engine", &EngineScreen};

    static MenuItemNumber<uint32_t> fpsItem{"FPS", &lastFps};
    static MenuItemNumber<uint32_t> frameTimeItem{"Frame Time", &lastFrameTime};

    static unsigned long long prevMillis1 = 0;

    static bool initialized = false;

    static void ensureInitialized()
    {
      if (initialized)
        return;
      initialized = true;

      settingsMenuItem.setTextColor(TFT_RED);
      settingsMenuItem.setBgColor(TFT_RED);

      menu.addMenuItem(&fpsItem);
      menu.addMenuItem(&frameTimeItem);

      menu.addMenuItem(&controlMenuItem);
      menu.addMenuItem(&settingsMenuItem);
      menu.addMenuItem(&remoteRelay);
      menu.addMenuItem(&boxThingEncoderItem);
      menu.addMenuItem(&testLoadingItem);
      menu.addMenuItem(&engineItem);
      menu.addMenuItem(&powerOffMenuItem);
      menu.addMenuItem(&testPopupItem);
      menu.addMenuItem(&testBlinkItem);
    }
  }

  void draw()
  {
    menu.draw();
  }

  void update()
  {
    menu.update();
  }

  void onEnter()
  {
    ensureInitialized();
    btnLed.SetColor565(TFT_BLUE);
    prevMillis1 = millis();
  }

  void onExit()
  {
    btnLed.Off();
  }

} // namespace HomeScreenNamespace

const Screen2 HomeScreen = {
    .name = "Home",
    .draw = HomeScreenNamespace::draw,
    .update = HomeScreenNamespace::update,
    .onEnter = HomeScreenNamespace::onEnter,
    .onExit = HomeScreenNamespace::onExit,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_WHITE,
};
