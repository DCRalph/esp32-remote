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

  MenuItem testItem1 = MenuItem("Test 1");
  MenuItem testItem2 = MenuItem("Test 2");
  MenuItem testItem3 = MenuItem("Test 3");

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

  // PowerOffMenuItem.setHidden(true);

  ControlMenuItem.addRoute(3, "CarLocks");

  menu.addMenuItem(&ControlMenuItem);
  menu.addMenuItem(&settingsMenuItem);
  menu.addMenuItem(&PowerOffMenuItem);
  menu.addMenuItem(&testPopupItem);
  menu.addMenuItem(&testBlinkItem);
  menu.addMenuItem(&testItem1);
  menu.addMenuItem(&testItem2);
  menu.addMenuItem(&testItem3);
}

void HomeScreen::draw()
{
  // display.noTopBar();
  menu.draw();
}

void HomeScreen::update()
{
  menu.update();

  if (ClickButtonTRIGGER.clicks == 1)
  {
    uint32_t color = (0 << 16) | (255 << 8) | 0;
    btnLed.Blink(color, 200, 3);
  }

  if (millis() - prevMillis1 > 10)
  {

    // String str = "";
    // switch (wakeup_reason)
    // {
    // case ESP_SLEEP_WAKEUP_UNDEFINED:
    //   str += "UNDEFINED";
    //   break;
    // case ESP_SLEEP_WAKEUP_ALL:
    //   str += "ALL";
    //   break;
    // case ESP_SLEEP_WAKEUP_EXT0:
    //   str += "EXT0";
    //   break;
    // case ESP_SLEEP_WAKEUP_EXT1:
    //   str += "EXT1";
    //   break;
    // case ESP_SLEEP_WAKEUP_TIMER:
    //   str += "TIMER";
    //   break;
    // case ESP_SLEEP_WAKEUP_TOUCHPAD:
    //   str += "TOUCHPAD";
    //   break;
    // case ESP_SLEEP_WAKEUP_ULP:
    //   str += "ULP";
    //   break;
    // case ESP_SLEEP_WAKEUP_GPIO:
    //   str += "GPIO";
    //   break;
    // case ESP_SLEEP_WAKEUP_UART:
    //   str += "UART";
    //   break;
    // case ESP_SLEEP_WAKEUP_WIFI:
    //   str += "WIFI";
    //   break;
    // case ESP_SLEEP_WAKEUP_COCPU:
    //   str += "COCPU";
    //   break;
    // case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG:
    //   str += "COCPU_TRAP_TRIG";
    //   break;
    // case ESP_SLEEP_WAKEUP_BT:
    //   str += "BT";
    //   break;
    // default:
    //   str += "Unknown";
    //   break;
    // }
  }
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