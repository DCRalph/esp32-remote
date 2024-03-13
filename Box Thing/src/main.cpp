#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>

#include "config.h"
#include "Setup/InitOTA.h"

#include "IO/GPIO.h"
#include "IO/Display.h"
#include "IO/ScreenManager.h"

#include "Screens/StartUp.h"
#include "Screens/UpdateProgress.h"

#include "Screens/Home.h"
#include "Screens/Settings.h"

#include "Screens/WiFiSettings.h"
#include "Screens/IOTest.h"

#include "Screens/WiFiInfo.h"

StartUpScreen startUp("Start Up");
UpdateProgressScreen updateProgress("Update Progress");

HomeScreen homeScreen("Home");
SettingsScreen settings("Settings");

WiFiSettingsScreen wifiSettings("WiFi Settings");
IOTestScreen ioTest("IO Test");

WiFiInfoScreen wifiInfo("WiFi Info");

unsigned long ms = 0;
char buffer[100];

void setup()
{
  Serial.begin(BAUD_RATE);
  Serial.println("[INFO] [SETUP] Starting...");
  Serial.println("[INFO] [SETUP] IOInit...");
  GPIO::initIO();

  Serial.println("[INFO] [SETUP] Display...");

  display.init();
  screenManager.init();

  screenManager.addScreen(&startUp);
  screenManager.addScreen(&updateProgress);

  screenManager.addScreen(&homeScreen);
  screenManager.addScreen(&settings);

  screenManager.addScreen(&wifiSettings);
  screenManager.addScreen(&ioTest);

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
  ClickButtonEnc.Update();

  if (ClickButtonEnc.clicks == -3)
    latch.Off();

  display.display();
}
