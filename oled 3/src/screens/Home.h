#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include "IO/GPIO.h"

class HomeScreen : public Screen
{
public:
  HomeScreen(String _name);

  Menu menu = Menu();

  MenuItemNavigate ControlMenuItem = MenuItemNavigate("Control", "Control");
  MenuItemNavigate settingsMenuItem = MenuItemNavigate("Settings", "Settings");
  MenuItemAction PowerOffMenuItem = MenuItemAction("Power Off", 2, []()
                                                   { esp_deep_sleep_start(); });

  MenuItemAction testPopupItem = MenuItemAction("Test Popup", 2, []()
                                                { screenManager.showPopup(new Popup("Test Popup", "This is a test gyY9 popup. pls wrap words TTTTT")); });

  MenuItemAction testBlinkItem = MenuItemAction("Test Blink", 2, []()
                                                {
                                                  uint32_t color = (0 << 16) | (255 << 8) | 0;
                                                  btnLed.Blink(color, 200, 3);
                                                  //
                                                });

  MenuItemNavigate remoteRelay = MenuItemNavigate("Remote Relay", "RemoteRelay");

  MenuItemNumber<uint32_t> fpsItem = MenuItemNumber<uint32_t>("FPS", &lastFps);
  MenuItemNumber<uint32_t> frameTimeItem = MenuItemNumber<uint32_t>("Frame Time", &lastFrameTime);

  void draw() override;
  void update() override;
  void onEnter() override;
  void onExit() override;

  unsigned long long prevMillis1;
};

HomeScreen::HomeScreen(String _name) : Screen(_name)
{
  topBarColor = TFT_BLUE;
  topBarTextColor = TFT_WHITE;

  settingsMenuItem.setTextColor(TFT_RED);
  settingsMenuItem.setBgColor(TFT_RED);

  ControlMenuItem.addRoute(3, "CarLocks");

  menu.addMenuItem(&fpsItem);
  menu.addMenuItem(&frameTimeItem);

  menu.addMenuItem(&ControlMenuItem);
  menu.addMenuItem(&settingsMenuItem);
  menu.addMenuItem(&remoteRelay);
  menu.addMenuItem(&PowerOffMenuItem);
  menu.addMenuItem(&testPopupItem);
  menu.addMenuItem(&testBlinkItem);
}

void HomeScreen::draw()
{
  // display.noTopBar();
  menu.draw();
}

void HomeScreen::update()
{
  menu.update();

  // if (ClickButtonTRIGGER.clicks == 1)
  // {
  //   uint32_t color = (0 << 16) | (255 << 8) | 0;
  //   btnLed.Blink(color, 200, 3);
  // }

}

void HomeScreen::onEnter()
{
  btnLed.SetColor565(topBarColor);
  prevMillis1 = millis();
}

void HomeScreen::onExit()
{
  btnLed.Off();
}