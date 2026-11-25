#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <Mesh.h>
#include <Wireless.h>
#include <dcr_display.h>

#include <dcr_Console.h>
#include <dcr_FatalHandler.h>
#include <dcr_Files.h>
#include <dcr_HttpRequest.h>
#include <dcr_Logger.h>
#include <dcr_NetLink.h>
#include <dcr_TaskManager.h>
#include <dcr_TimeProfiler.h>

#include "config.h"
#include "Setup/InitOTA.h"

#include "IO/Battery.h"
#include "IO/GPIO.h"
#include "IO/Haptic.h"
#include "IO/TopBar.h"
#include "IO/BoxThingDisplay.h"

#include "Screens/Screens.h"

#include "Screens/StartUp.h"
#include "Screens/UpdateProgress.h"

#include "Screens/Shutdown.h"

#include "Screens/Debug/Battery.h"
#include "Screens/Debug/IOTest.h"
#include "Screens/Debug/Debug.h"

#include "Screens/Control/Car.h"
#include "Screens/Control/EncoderTransmiter.h"
#include "Screens/Control/EspnowSwitch.h"
#include "Screens/Control/RemoteRelay.h"
#include "Screens/Control/ServosControl.h"

#include "Screens/Settings/GeneralSettings.h"
#include "Screens/Settings/WiFi/WiFiInfo.h"
#include "Screens/Settings/WiFi/WiFiPassword.h"
#include "Screens/Settings/WiFi/WiFiScan.h"
#include "Screens/Settings/WiFiSettings.h"

#include "Screens/Home.h"
#include "Screens/SwitchMenu.h"
#include "Screens/Settings.h"

#undef LOG_TAG
#define LOG_TAG "MAIN"

unsigned long batteryLoopMs = 0;

uint64_t lastDraw = 0;

void espNowCb(WirelessFrame *frame)
{
  if (frame->packet.type == CMD_PING)
    RemoteRelay::notePingReply();
}

void beginMeshTransport()
{
  auto *sync = SyncManager::getInstance();
  sync->setTransport(&wireless);
  sync->setDeviceIdProvider([]()
                            {
                              uint64_t mac = ESP.getEfuseMac();
                              return static_cast<uint32_t>(mac ^ (mac >> 32));
                            });
  sync->setModePersistence(
      []()
      {
        if (!preferences.isKey("meshSyncMode"))
          return static_cast<uint8_t>(SyncMode::JOIN);
        return preferences.getUChar("meshSyncMode", static_cast<uint8_t>(SyncMode::JOIN));
      },
      [](uint8_t m)
      {
        preferences.putUChar("meshSyncMode", m);
      });
  sync->begin();
}

void teardownMeshTransport()
{
  wireless.unSetup();
  SyncManager::getInstance()->setTransport(nullptr);
}

void setupWiFi()
{
  StartUp::setState(StartUpState::ConnectingWifi);
  display.render();

  debugI("WiFi...");

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
  callbacks.onConnected = [](const WiFiDetails &details)
  {
    debugI("Connected to %s as %s", details.ssid.c_str(), details.ip.toString().c_str());
  };
  callbacks.onConnectFailed = [](const String &reason)
  {
    debugW("Connect failed: %s", reason.c_str());
  };
  netLink.setCallbacks(callbacks);

  netLink.init();

  HTTP::init();
  HTTP::setUserAgent("BoxThingV2/1.0");
}

void setupESPNOW()
{
  beginMeshTransport();
  StartUp::setState(StartUpState::EspNowStarted);
}

// Sources are polled only when a menu, popup or widget asks for input, so the
// haptics fire exactly when the library consumes a turn or a click.
EncoderInput encoderInput({
    .readDelta = []() -> int32_t
    {
      const int32_t delta = (int32_t)encoderGetCount();
      if (delta != 0)
      {
        encoderClearCount();
        haptic.playEffect(4);
      }
      return delta;
    },
    .readClicks = []()
    {
      const int clicks = (int)ClickButtonEnc.clicks;
      if (clicks != 0)
        haptic.playEffect(40);
      return clicks;
    },
});

void setupDisplay()
{
  display.begin(displayDriver);
  display.setTopBarRenderer(TopBar::render);
  Inputs::addSource(encoderInput);
}

void setup()
{
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH); // Set latch pin to high as soon as possible

  Serial.begin(BAUD_RATE);
  Serial.setTimeout(10);
  Console::addStream(&Serial);

  Logger::SetLevel(LogLevel::Debug);
  Logger::InstallLogHook();
  FatalHandler::install();
  taskManager.begin();
  timeProfiler.begin(2048, 1, 0);
  Files::init();

  initConfig();

  setupDisplay();
  screenManager.setScreen(&StartUpScreen2);
  screenManager.applyPendingScreenChange();
  StartUp::setStage(1);
  display.render();

  debugI("Starting...");
  debugI("IOInit...");
  GpIO::initIO();
  haptic.init();

  StartUp::setStage(2);
  display.render();

  if (preferences.getBool("espnowOn", false))
  {
    setupESPNOW();
  }
  else
  {
    setupWiFi();
    InitOta();
  }

  wireless.setOnReceiveOther(espNowCb);

  StartUp::setStage(3);
  display.render();

  debugI("Done");

  do
  {
    ClickButtonEnc.Update();
  } while (ClickButtonEnc.depressed);

  if (StartUp::getState() != StartUpState::ApStarted)
  {
    screenManager.setScreen(&HomeScreen2);
    haptic.playEffect(37);
  }
}

void loop()
{
  if (wireless.isSetupDone())
    SyncManager::getInstance()->loop();
  else
    netLink.loop();

  if (otaSetup)
    ArduinoOTA.handle();
  else if (WiFi.status() == WL_CONNECTED && !otaSetup)
    InitOta();

  if (millis() - batteryLoopMs > 500)
  {
    batteryLoopMs = millis();
    batteryUpdate();
  }

  const Screen2 *current = screenManager.getCurrentScreen();
  const bool autoOffExempt = current == &ShutdownScreen2 ||
                             current == &StartUpScreen2 ||
                             current == &UpdateProgressScreen2;

  if (autoOffMin != 0 && !autoOffExempt &&
      millis() - lastInteract > autoOffMin * 60000)
  {
    debugI("Auto off timer");
    screenManager.setScreen(&ShutdownScreen2);
  }

  if (ClickButtonEnc.clicks == -3)
    screenManager.setScreen(&ShutdownScreen2);

  if (millis() - lastDraw > 25)
  {
    lastDraw = millis();

    ClickButtonEnc.Update();

    timeProfiler.increment(PROFILE_FRAME);
    timeProfiler.start(PROFILE_FRAME);
    display.render();
    timeProfiler.stop(PROFILE_FRAME);
  }
}
