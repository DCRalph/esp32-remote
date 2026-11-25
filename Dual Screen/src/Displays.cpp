#include "Displays.h"

#include <ILI9341DisplayDriver.h>
#include <TFTDisplayDriver.h>
#include <dcr_TimeProfiler.h>

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
  timeProfiler.increment(PROFILE_LCD);
  timeProfiler.start(PROFILE_LCD);
  display.render();
  timeProfiler.stop(PROFILE_LCD);

  timeProfiler.increment(PROFILE_TOUCH);
  timeProfiler.start(PROFILE_TOUCH);
  touchDisplay.render();
  timeProfiler.stop(PROFILE_TOUCH);
}
