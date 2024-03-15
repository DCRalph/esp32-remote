#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>

#include "config.h"
#include "Setup/InitOTA.h"

#include "IO/MQTT.h"
#include "IO/GPIO.h"
#include "IO/Display.h"
#include "IO/ScreenManager.h"

#include "Screens/StartUp.h"
#include "Screens/UpdateProgress.h"

#include "Screens/Debug/Debug.h"
#include "Screens/Debug/IOTest.h"

#include "Screens/Home.h"
#include "Screens/SwitchMenu.h"
#include "Screens/Settings.h"

#include "Screens/Switch/MQTTSwitch.h"

#include "Screens/Settings/GeneralSettings.h"
#include "Screens/Settings/DisplaySettings.h"
#include "Screens/Settings/WiFiSettings.h"
#include "Screens/Settings/MQTTInfo.h"

#include "Screens/Settings/WiFi/WiFiInfo.h"

StartUpScreen startUp("Start Up");
UpdateProgressScreen updateProgress("Update Progress");

// #### /Debug
DebugScreen debug("Debug");
IOTestScreen ioTest("IO Test");

// #### /
HomeScreen homeScreen("Home");
SwitchMenuScreen switchMenu("Switch Menu");
SettingsScreen settings("Settings");

// #### /Switch
MQTTSwitchScreen mqttSwitch("MQTT Switch");

// #### /Settings
GeneralSettingsScreen generalSettings("General Settings");
DisplaySettingsScreen displaySettings("Display Settings");
WiFiSettingsScreen wifiSettings("WiFi Settings");
MQTTInfoScreen mqttInfo("MQTT Info");

// #### /Settings/WiFi
WiFiInfoScreen wifiInfo("WiFi Info");

unsigned long ms = 0;
char buffer[100];

void setup()
{
  initConfig();
  Serial.println("[INFO] [SETUP] Starting...");
  Serial.println("[INFO] [SETUP] IOInit...");
  GPIO::initIO();

  Serial.println("[INFO] [SETUP] Display...");

  display.init();
  screenManager.init();

  screenManager.addScreen(&startUp);
  screenManager.addScreen(&updateProgress);

  // #### /Debug
  screenManager.addScreen(&debug);
  screenManager.addScreen(&ioTest);

  // #### /
  screenManager.addScreen(&homeScreen);
  screenManager.addScreen(&switchMenu);
  screenManager.addScreen(&settings);

  // #### /Switch
  screenManager.addScreen(&mqttSwitch);

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

  Serial.println("[INFO] [SETUP] OTA...");
  InitOta();

  Serial.println("[INFO] [SETUP] Done");
  Serial.println();

  if (((StartUpScreen *)screenManager.getCurrentScreen())->getState() != StartUpState::ApStarted)
    screenManager.setScreen("Home");
}

void loop()
{
  wm.process();
  ArduinoOTA.handle();
  mqtt.loop();
  ClickButtonEnc.Update();

  if (ClickButtonEnc.clicks == -3)
    latch.Off();

  display.display();
}
