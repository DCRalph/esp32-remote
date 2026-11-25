#include <Arduino.h>

#include <TFTDisplayDriver.h>
#include <dcr_display.h>

#include <dcr_Console.h>
#include <dcr_FatalHandler.h>
#include <dcr_Logger.h>
#include <dcr_TaskManager.h>
#include <dcr_TimeProfiler.h>

#include "Device.h"
#include "Touch.h"
#include "config.h"
#include "screens/Screens.h"

#undef LOG_TAG
#define LOG_TAG "MAIN"

namespace
{
  bool calibrationRequested = false;
  uint32_t lastStatsMs = 0;

  // Built on first use: the rotation comes from NVS, read in Device::begin().
  TFTDisplayDriver &panel()
  {
    static TFTDisplayDriver driver(LCD_WIDTH, LCD_HEIGHT, Device::info().rotation);
    return driver;
  }

  void runCalibration()
  {
    calibrationRequested = false;
    Touch::calibrate(panel().tft());
    // Calibration drew straight to the panel: push the whole next frame.
    panel().resync();
    display.invalidate();
    Touch::input.suppressUntilRelease();
  }

  void logStats()
  {
    if (millis() - lastStatsMs < 1000)
      return;
    lastStatsMs = millis();
    const FrameStats &s = display.stats();
    debugI("%2ufps draw %.1fms push %.1fms (%u%%) max %.1fms load %u%% | heap %uK psram %uK", s.fps,
           s.avgDrawUs / 1000.0f, s.avgPushUs / 1000.0f, s.avgPushedPercent, s.maxFrameUs / 1000.0f,
           s.loadPercent, ESP.getFreeHeap() / 1024, ESP.getFreePsram() / 1024);
  }
}

void App::requestCalibration()
{
  calibrationRequested = true;
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(10);
  Console::addStream(&Serial);
  Logger::SetLevel(LogLevel::Debug);
  Logger::InstallLogHook();

  // Early: the latch keeps the board powered once the button is let go.
  Device::begin();

  FatalHandler::install();
  taskManager.begin();
  timeProfiler.begin(2048, 1, 0);

  display.begin(panel());
  Touch::begin(panel().tft());
  Inputs::addSource(Touch::input);
  screenManager.setScreen(&HomeScreen);

  // Light the backlight only once a real frame is on the panel.
  display.render();
  Device::setBacklight(true);
  debugI("Ready");
}

void loop()
{
  const uint32_t frameStartMs = millis();

  Device::loop();
  display.render();
  // Catches a tap that starts and ends while the frame is drawing.
  Touch::input.sample();

  if (calibrationRequested)
    runCalibration();

  logStats();

  // Sleep out the rest of the frame. Not xTaskDelayUntil: after a slow frame it
  // runs catch-up frames back to back, which shows as a burst.
  const uint32_t spentMs = millis() - frameStartMs;
  if (spentMs < FRAME_INTERVAL_MS)
    vTaskDelay(pdMS_TO_TICKS(FRAME_INTERVAL_MS - spentMs));
}
