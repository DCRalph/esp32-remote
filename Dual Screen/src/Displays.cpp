#include "Displays.h"

#include <ILI9341DisplayDriver.h>
#include <TFTDisplayDriver.h>

#undef LOG_TAG
#define LOG_TAG "PERF"
#include <dcr_Logger.h>

#include "config.h"
#include "IO/Battery.h"
#include "IO/Buttons.h"
#include "IO/Touch.h"
#include "screens/Screens.h"

Display touchDisplay;
ScreenManager touchScreens;

namespace
{
  TFTDisplayDriver lcdDriver(LCD_WIDTH, LCD_HEIGHT, LCD_ROTATION);

  // Buffered: 150 KB frame in PSRAM, pushed in one SPI burst, so no flicker.
  ILI9341DisplayDriver touchDriver(FSPI,
                                   ILI9341DisplayDriver::Pins{TouchPanelPins::kCS, TouchPanelPins::kDC,
                                                              TouchPanelPins::kRST, TouchPanelPins::kSCLK,
                                                              TouchPanelPins::kMISO, TouchPanelPins::kMOSI},
                                   1, 60000000, true);

  // One click moves, a double click selects, a triple click backs out of UI test screens.
  ButtonInput::Config buttonConfig()
  {
    ButtonInput::Config c;
    c.up = []()
    { return (int)ClickButtonUP.clicks; };
    c.down = []()
    { return (int)ClickButtonDOWN.clicks; };
    c.defaultSelectClicks = 2;
    c.multiClickSelects = true;
    return c;
  }
  ButtonInput buttonInput(buttonConfig());

  int batteryPercent()
  {
    return battery.getPercentageI();
  }

  String describe(const FrameStats &s)
  {
    char buf[128];
    snprintf(buf, sizeof(buf), "%2ufps %3u/s draw %.1fms push %.1fms (%u%%) max %.1fms upd %.2fms load %u%%",
             s.fps, s.renders, s.avgDrawUs / 1000.0f, s.avgPushUs / 1000.0f, s.avgPushedPercent,
             s.maxFrameUs / 1000.0f, s.avgUpdateUs / 1000.0f, s.loadPercent);
    return buf;
  }

  uint32_t lastLogMs = 0;
}

void Displays::begin()
{
  display.begin(lcdDriver);
  display.setBatteryProvider(batteryPercent);
  display.addInput(buttonInput);

  touchDisplay.begin(touchDriver, &touchScreens);
  // Each frame is a 150 KB SPI push; input, popups and screen changes still redraw at once.
  touchDisplay.setIdleRedrawMs(200);
  Touch::begin(touchDriver.spi());
  touchDisplay.addInput(Touch::input);

  screenManager.setScreen(&HomeScreen);
  touchScreens.setScreen(&TouchHomeScreen);
}

void Displays::render()
{
  display.render();
  // Catches a tap that starts and ends while the LCD frame is drawing.
  Touch::input.sample();
  touchDisplay.render();
}

void Displays::logStats()
{
  if (millis() - lastLogMs < 1000)
    return;
  lastLogMs = millis();
  debugI("LCD %s | Touch %s | heap %uK psram %uK", describe(display.stats()).c_str(),
         describe(touchDisplay.stats()).c_str(), ESP.getFreeHeap() / 1024, ESP.getFreePsram() / 1024);
}
