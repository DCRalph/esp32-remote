#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>

#include "config.h"
#include "Setup/InitOTA.h"

#include "IO/GPIO.h"
#include "IO/Display.h"
#include "IO/ScreenManager.h"
#include "IO/Wireless.h"
#include "IO/Battery.h"

#include "Screens/StartUp.h"
#include "Screens/UpdateProgress.h"
#include "Screens/Shutdown.h"

#include "Screens/Debug/Debug.h"
#include "Screens/Debug/IOTest.h"
#include "Screens/Debug/Battery.h"

#include "Screens/Home.h"
#include "Screens/SwitchMenu.h"
#include "Screens/Settings.h"

#include "Screens/Control/EspnowSwitch.h"
#include "Screens/Control/Car.h"
#include "Screens/Control/CarFlash.h"

#include "Screens/Settings/GeneralSettings.h"
#include "Screens/Settings/WiFiSettings.h"

#include "Screens/Settings/WiFi/WiFiInfo.h"

StartUpScreen startUp("Start Up");
UpdateProgressScreen updateProgress("Update Progress");
ShutdownScreen shutdown("Shutdown");

// #### /Debug
DebugScreen debug("Debug");
IOTestScreen ioTest("IO Test");
BatteryScreen battery("Battery");

// #### /
HomeScreen homeScreen("Home");
SwitchMenuScreen switchMenu("Switch Menu");
SettingsScreen settings("Settings");

// #### /Control
EspnowSwitchScreen espnowSwitch("Espnow Switch");
CarControlScreen carControl("Car Control");
CarFlashScreen carFlash("Car Flash");

// #### /Settings
GeneralSettingsScreen generalSettings("General Settings");
WiFiSettingsScreen wifiSettings("WiFi Settings");

// #### /Settings/WiFi
WiFiInfoScreen wifiInfo("WiFi Info");

unsigned long batteryLoopMs = 0;
char buffer[100];

void setupWiFi()
{
  ((StartUpScreen *)screenManager.getCurrentScreen())->setState(StartUpState::ConnectingWifi);
  display.display();

  Serial.println("[INFO] [SETUP] WiFi...");
  WiFi.mode(WIFI_STA);

  wm.setConfigPortalBlocking(false);

  if (wm.autoConnect(AP_SSID))
  {
    Serial.println("[INFO] [SETUP] Connected");
  }
  else
  {
    Serial.println("[INFO] [SETUP] Config portal running");
    ((StartUpScreen *)screenManager.getCurrentScreen())->setState(StartUpState::ApStarted);
    display.display();
  }
}

void espNowCb(fullPacket *fp)
{
  if (fp->p.type == CMD_RELAY_1_GET)
    globalRelay1 = fp->p.data[0];
  else if (fp->p.type == CMD_RELAY_2_GET)
    globalRelay2 = fp->p.data[0];
  else if (fp->p.type == CMD_RELAY_3_GET)
    globalRelay3 = fp->p.data[0];
  else if (fp->p.type == CMD_RELAY_4_GET)
    globalRelay4 = fp->p.data[0];
  else if (fp->p.type == CMD_RELAY_5_GET)
    globalRelay5 = fp->p.data[0];
  else if (fp->p.type == CMD_RELAY_6_GET)
    globalRelay6 = fp->p.data[0];

  else if (fp->p.type == CMD_RELAY_ALL)
  {
    globalRelay1 = fp->p.data[0];
    globalRelay2 = fp->p.data[1];
    globalRelay3 = fp->p.data[2];
    globalRelay4 = fp->p.data[3];
    globalRelay5 = fp->p.data[4];
    globalRelay6 = fp->p.data[5];
  }
}

void setupESPNOW()
{
  wireless.setup();
  ((StartUpScreen *)screenManager.getCurrentScreen())->setState(StartUpState::EspNowStarted);

  wireless.setRecvCb(espNowCb);
}

void setup()
{
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH); // Set latch pin to high as soon as possible

  initConfig();
  Serial.println("[INFO] [SETUP] Starting...");
  Serial.println("[INFO] [SETUP] IOInit...");
  GpIO::initIO();

  Serial.println("[INFO] [SETUP] Display...");

  display.init();
  screenManager.init();

  screenManager.addScreen(&startUp);
  screenManager.addScreen(&updateProgress);
  screenManager.addScreen(&shutdown);

  // #### /Debug
  screenManager.addScreen(&debug);
  screenManager.addScreen(&ioTest);
  screenManager.addScreen(&battery);

  // #### /
  screenManager.addScreen(&homeScreen);
  screenManager.addScreen(&switchMenu);
  switchMenu.setTopBarText("Control");
  screenManager.addScreen(&settings);

  // #### /Control
  screenManager.addScreen(&espnowSwitch);
  espnowSwitch.setTopBarText("ESPNOW");
  screenManager.addScreen(&carControl);
  carControl.setTopBarText("Car");
  screenManager.addScreen(&carFlash);
  carFlash.setTopBarText("Flash");

  // #### /Settings
  screenManager.addScreen(&generalSettings);
  generalSettings.setTopBarText("General");
  screenManager.addScreen(&wifiSettings);
  wifiSettings.setTopBarText("Wi-Fi");

  // #### /Settings/WiFi
  screenManager.addScreen(&wifiInfo);

  screenManager.setScreen("Start Up");
  display.display();

  if (preferences.getBool("espnowOn", false))
  {
    setupESPNOW();
  }
  else
  {
    setupWiFi();
    InitOta();
  }

  Serial.println("[INFO] [SETUP] Done");
  Serial.println();

  do
  {
    ClickButtonEnc.Update();
  } while (ClickButtonEnc.depressed);

  if (((StartUpScreen *)screenManager.getCurrentScreen())->getState() != StartUpState::ApStarted)
    screenManager.setScreen("Home");
}

void loop()
{
  if (!wireless.isSetupDone())
    wm.process();

  ClickButtonEnc.Update();

  if (otaSetup)
    ArduinoOTA.handle();
  else if (WiFi.status() == WL_CONNECTED && !otaSetup)
    InitOta();

  if (millis() - batteryLoopMs > 500)
  {
    batteryLoopMs = millis();
    batteryUpdate();
  }

  if (autoOffMin != 0 &&
      millis() - lastInteract > autoOffMin * 60000 && screenManager.getCurrentScreen()->name != "Shutdown" &&
      screenManager.getCurrentScreen()->name != "Start Up" &&
      screenManager.getCurrentScreen()->name != "Update Progress")
  {
    Serial.println("Auto off Timer");
    screenManager.setScreen("Shutdown");
  }

  if (ClickButtonEnc.clicks != 0)
    lastInteract = millis();

  if (ClickButtonEnc.clicks == -3)
    screenManager.setScreen("Shutdown");

  display.display();
}
