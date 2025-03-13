#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "Setup/InitOTA.h"

#include "IO/GPIO.h"
#include "IO/Display.h"
#include "IO/ScreenManager.h"
#include "IO/Wireless.h"
#include "IO/Battery.h"
#include "IO/Haptic.h"

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
// #include "Screens/Control/CarFlash.h"
#include "Screens/Control/RemoteRelay.h"
#include "Screens/Control/EncoderTransmiter.h"
#include "Screens/Control/ServosControl.h"

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
// CarFlashScreen carFlash("Car Flash");
RemoteRelayScreen remoteRelay("Remote Relay");
EncoderTransmiterScreen encoderTransmiter("Encoder Transmiter");
ServoControlScreen servosControl("Servos Control");

// #### /Settings
GeneralSettingsScreen generalSettings("General Settings");
WiFiSettingsScreen wifiSettings("WiFi Settings");

// #### /Settings/WiFi
WiFiInfoScreen wifiInfo("WiFi Info");

unsigned long batteryLoopMs = 0;
char buffer[100];

TaskHandle_t fpsTask;
uint64_t lastDraw = 0;

void fpsTaskFunction(void *pvParameters)
{
  for (;;)
  {
    lastFps = fps;
    fps = 0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

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
  if (fp->p.type == CMD_PING)
  {
    remoteRelay.lastConfirmedPing = millis();
  }
}

void setupESPNOW()
{
  wireless.setup();
  ((StartUpScreen *)screenManager.getCurrentScreen())->setState(StartUpState::EspNowStarted);
}

void setup()
{
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH); // Set latch pin to high as soon as possible

  initConfig();

  display.init();
  screenManager.init();

  screenManager.addScreen(&startUp);

  screenManager.setScreen("Start Up");
  display.display();

  Serial.println("[INFO] [SETUP] Starting...");
  Serial.println("[INFO] [SETUP] IOInit...");
  GpIO::initIO();
  haptic.init();

  ((StartUpScreen *)screenManager.getCurrentScreen())->setStage(1);
  display.display();

  Serial.println("[INFO] [SETUP] Display...");

  screenManager.addScreen(&updateProgress);
  screenManager.addScreen(&shutdown);

  ((StartUpScreen *)screenManager.getCurrentScreen())->setStage(2);
  display.display();

  // #### /Debug
  screenManager.addScreen(&debug);
  screenManager.addScreen(&ioTest);
  screenManager.addScreen(&battery);

  ((StartUpScreen *)screenManager.getCurrentScreen())->setStage(3);
  display.display();

  // #### /
  screenManager.addScreen(&homeScreen);
  screenManager.addScreen(&switchMenu);
  switchMenu.setTopBarText("Control");
  screenManager.addScreen(&settings);

  ((StartUpScreen *)screenManager.getCurrentScreen())->setStage(4);
  display.display();

  // #### /Control
  screenManager.addScreen(&espnowSwitch);
  espnowSwitch.setTopBarText("ESPNOW");
  screenManager.addScreen(&carControl);
  carControl.setTopBarText("Car");
  // screenManager.addScreen(&carFlash);
  // carFlash.setTopBarText("Flash");
  screenManager.addScreen(&remoteRelay);
  remoteRelay.setTopBarText("Relay");
  screenManager.addScreen(&encoderTransmiter);
  encoderTransmiter.setTopBarText("Encoder");
  screenManager.addScreen(&servosControl);
  servosControl.setTopBarText("Servos");

  ((StartUpScreen *)screenManager.getCurrentScreen())->setStage(5);
  display.display();

  // #### /Settings
  screenManager.addScreen(&generalSettings);
  generalSettings.setTopBarText("General");
  screenManager.addScreen(&wifiSettings);
  wifiSettings.setTopBarText("Wi-Fi");

  ((StartUpScreen *)screenManager.getCurrentScreen())->setStage(6);
  display.display();

  // #### /Settings/WiFi
  screenManager.addScreen(&wifiInfo);

  ((StartUpScreen *)screenManager.getCurrentScreen())->setStage(7);
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

  wireless.setOnReceiveOther(espNowCb);

  Serial.println("[INFO] [SETUP] Done");
  Serial.println();

  xTaskCreatePinnedToCore(
      fpsTaskFunction, /* Task function. */
      "fpsTask",       /* name of task. */
      10000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      1,               /* priority of the task */
      &fpsTask,        /* Task handle to keep track of created task */
      0);

  do
  {
    ClickButtonEnc.Update();
  } while (ClickButtonEnc.depressed);

  if (((StartUpScreen *)screenManager.getCurrentScreen())->getState() != StartUpState::ApStarted)
  {
    screenManager.setScreen("Home");
    haptic.playEffect(37);
  }
}

int64_t lastEncoderValue = 0;

void loop()
{
  fps++;

  if (!wireless.isSetupDone())
    wm.process();

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

  if (ClickButtonEnc.clicks == -3)
    screenManager.setScreen("Shutdown");

  // display.display();
  if (millis() - lastDraw > 20)
  {
    lastDraw = millis();

    ClickButtonEnc.Update();

    frameTime = millis();
    display.display();
    lastFrameTime = millis() - frameTime;
  }
}
