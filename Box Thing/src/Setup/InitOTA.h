#pragma once

#include "config.h"
#include <ArduinoOTA.h>

#include "IO/Display.h"
#include "Screens/UpdateProgress.h"

int lastPercent = 0;

void InitOta()
{
  Serial.println("\t[INFO] [OTA] Initializing...");

  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.onStart([]()
                     {
                 String type = (ArduinoOTA.getCommand() == U_FLASH) ? "Sketch" : "Filesystem";
                 Serial.println("\n\n\nStart updating " + type);

                  screenManager.setScreen("Update Progress");
                  ((UpdateProgressScreen *)screenManager.getCurrentScreen())->setProgress(0);
                  display.display(); });

  ArduinoOTA.onEnd([]()
                   { 
              Serial.println("End. Rebooting!"); 

              screenManager.setScreen("Update Progress");
              ((UpdateProgressScreen *)screenManager.getCurrentScreen())->setMessage("Rebooting...");
              display.display();

              delay(1000);

              ESP.restart(); });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        {
                          u8 percent = (progress / (total / 100));

                          if (percent == lastPercent)
                            return;
                          lastPercent = percent;

                          Serial.println("Progress: " + String(percent) + "%");

                          ((UpdateProgressScreen *)screenManager.getCurrentScreen())->setProgress(percent); 
                          display.display(); });

  ArduinoOTA.onError([](ota_error_t error)
                     {
                       if (error == OTA_AUTH_ERROR)
                         Serial.println("Auth Failed");
                       else if (error == OTA_BEGIN_ERROR)
                         Serial.println("Begin Failed");
                       else if (error == OTA_CONNECT_ERROR)
                         Serial.println("Connect Failed");
                       else if (error == OTA_RECEIVE_ERROR)
                         Serial.println("Receive Failed");
                       else if (error == OTA_END_ERROR)
                         Serial.println("End Failed");

                       screenManager.setScreen("Update Progress");
                       ((UpdateProgressScreen *)screenManager.getCurrentScreen())->setMessage("Error: " + String(error));
                       display.display();

                       delay(1000);
                     });

  ArduinoOTA.begin();

  Serial.println("\t[INFO] [OTA] Initialized");
}