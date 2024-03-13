#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "Setup/InitOTA.h"

#include "IO/GPIO.h"
#include "IO/Display.h"
#include "IO/ScreenManager.h"

#include "Screens/Home.h"
#include "Screens/MainMenu.h"
#include "Screens/WiFiInfo.h"
#include "Screens/UpdateProgress.h"

HomeScreen homeScreen("Home");
MainMenuScreen mainMenu("Main Menu");
WiFiInfoScreen wifiInfo("WiFi Info");
UpdateProgressScreen updateProgress("Update Progress");

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

  screenManager.addScreen(&homeScreen);
  screenManager.addScreen(&mainMenu);
  screenManager.addScreen(&wifiInfo);
  screenManager.addScreen(&updateProgress);

  Serial.println("[INFO] [SETUP] WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  // while (WiFi.waitForConnectResult() != WL_CONNECTED)
  // {
  //   Serial.println("Connection Failed! Rebooting...");
  //   delay(5000);
  //   ESP.restart();
  // }

  Serial.println("[INFO] [SETUP] OTA...");
  InitOta();

  Serial.println("[INFO] [SETUP] Done");
  Serial.println();

  screenManager.setScreen("Main Menu");
}

void loop()
{
  ArduinoOTA.handle();
  ClickButtonEnc.Update();

  if (ClickButtonEnc.clicks == -1)
  {
    latch.Off();
  }

  display.display();

  // display.u8g2.firstPage();
  // do
  // {
  //   display.u8g2.setFont(u8g2_font_logisoso16_tr);

  //   sprintf(buffer, "%s", WiFi.localIP().toString().c_str());
  //   display.u8g2.drawStr(0, 16, buffer);

  //   sprintf(buffer, "b: %d, s: %d", btn, sw);
  //   display.u8g2.drawStr(0, 34, buffer);

  //   sprintf(buffer, "bat: %.2f", battery);
  //   display.u8g2.drawStr(0, 52, buffer);

  // } while (display.u8g2.nextPage());
}
