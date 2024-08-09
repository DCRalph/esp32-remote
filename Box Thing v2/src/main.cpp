#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>

#include "config.h"
#include "Setup/InitOTA.h"

#include "IO/MQTT.h"
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

#include "Screens/Switch/MQTTSwitch.h"
#include "Screens/Switch/EspnowSwitch.h"

#include "Screens/Settings/GeneralSettings.h"
#include "Screens/Settings/DisplaySettings.h"
#include "Screens/Settings/WiFiSettings.h"
#include "Screens/Settings/MQTTInfo.h"

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

// #### /Switch
MQTTSwitchScreen mqttSwitch("MQTT Switch");
EspnowSwitchScreen espnowSwitch("Espnow Switch");

// #### /Settings
GeneralSettingsScreen generalSettings("General Settings");
DisplaySettingsScreen displaySettings("Display Settings");
WiFiSettingsScreen wifiSettings("WiFi Settings");
MQTTInfoScreen mqttInfo("MQTT Info");

// #### /Settings/WiFi
WiFiInfoScreen wifiInfo("WiFi Info");

unsigned long batteryLoopMs = 0;
char buffer[100];

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
  screenManager.addScreen(&settings);

  // #### /Switch
  screenManager.addScreen(&mqttSwitch);
  screenManager.addScreen(&espnowSwitch);

  // #### /Settings
  screenManager.addScreen(&generalSettings);
  screenManager.addScreen(&displaySettings);
  screenManager.addScreen(&wifiSettings);
  screenManager.addScreen(&mqttInfo);

  // #### /Settings/WiFi
  screenManager.addScreen(&wifiInfo);

  screenManager.setScreen("Start Up");
  display.display();

  ((StartUpScreen *)screenManager.getCurrentScreen())->setState(StartUpState::ConnectingWifi);
  display.display();

  Serial.println("[INFO] [SETUP] WiFi...");
  WiFi.mode(WIFI_STA);
  // WiFi.begin(WIFI_SSID, WIFI_PASS);
  wm.setConfigPortalBlocking(false);
  if (wm.autoConnect(AP_SSID))
  {
    Serial.println("[INFO] [SETUP] Connected");
  }
  else
  {
    Serial.println("[INFO] [SETUP] Configportal running");
    ((StartUpScreen *)screenManager.getCurrentScreen())->setState(StartUpState::ApStarted);
    display.display();
  }

  mqtt.init();
  // myEspnow.init();
  // wireless.setup();

  Serial.println("[INFO] [SETUP] OTA...");
  InitOta();

  Serial.println("[INFO] [SETUP] Done");
  Serial.println();

  if (((StartUpScreen *)screenManager.getCurrentScreen())->getState() != StartUpState::ApStarted)
    screenManager.setScreen("Home");
}

void loop()
{
  if (!wireless.isSetupDone())
    wm.process();

  ArduinoOTA.handle();
  mqtt.loop();
  ClickButtonEnc.Update();

  if (millis() - batteryLoopMs > 1000)
  {
    batteryLoopMs = millis();
    batteryUpdate();
  }

  if (ClickButtonEnc.clicks == -3)
    screenManager.setScreen("Shutdown");

  if (ClickButtonEnc.clicks == -2)
  {
    Serial.println("Battery voltage: " + String(batteryGetVoltage()) + "V");
  }

  display.display();
}
