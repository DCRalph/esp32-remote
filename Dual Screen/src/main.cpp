#include <Arduino.h>

#include <dcr_Console.h>
#include <dcr_FatalHandler.h>
#include <dcr_Files.h>
#include <dcr_Logger.h>
#include <dcr_NetLink.h>
#include <dcr_TaskManager.h>

#include "config.h"
#include "Displays.h"
#include "IO/Battery.h"
#include "IO/Buttons.h"

#if __has_include("secrets.h")
#include "secrets.h"
#endif

#undef LOG_TAG
#define LOG_TAG "MAIN"

namespace
{
  uint32_t lastBatteryMs = 0;

  void setupNetwork()
  {
    NetLinkCallbacks callbacks;
    callbacks.isBleActive = []()
    { return false; };
    callbacks.isCellularPreferred = []()
    { return false; };
    callbacks.currentUnixTime = [](bool &timeOk)
    {
      timeOk = false;
      return (uint32_t)(millis() / 1000);
    };
    netLink.setCallbacks(callbacks);
#ifdef FACTORY_WIFI_SSID
    netLink.setFactoryCredentials(FACTORY_WIFI_SSID, FACTORY_WIFI_PASSWORD);
#endif
    netLink.init();
  }
}

void setup()
{
  pinMode(POWER_ON_PIN, OUTPUT);
  digitalWrite(POWER_ON_PIN, HIGH);

  Serial.begin(115200);
  Serial.setTimeout(10);
  Console::addStream(&Serial);

  Files::init();
  Logger::SetLevel(LogLevel::Debug);
  Logger::InstallLogHook();
  FatalHandler::install();
  taskManager.begin();

  initConfig();

  // A wake press must not fall through into the UI as a click.
  while (!digitalRead(BTN_DOWN_PIN) || !digitalRead(BTN_UP_PIN))
    delay(5);
  buttons.setup();
  buttons.update();
  battery.init();

  Displays::begin();
  setupNetwork();
  debugI("Ready");
}

void loop()
{
  buttons.update();
  netLink.loop();

  if (millis() - lastBatteryMs > 500)
  {
    lastBatteryMs = millis();
    battery.update();
  }

  Displays::render();
  Displays::logStats();

  static TickType_t lastWake = xTaskGetTickCount();
  xTaskDelayUntil(&lastWake, pdMS_TO_TICKS(FRAME_INTERVAL_MS));
}
