#include "InitOTA.h"

bool otaSetup = false;
bool otaInProgress = false;

static int lastPercent = 0;

void InitOta()
{
  Serial.println("\t[INFO] [OTA] Initializing...");

  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.onStart([]()
                     {
                       otaInProgress = true;

                       String type = (ArduinoOTA.getCommand() == U_FLASH) ? "Sketch" : "Filesystem";
                       Serial.println("\n\n\nStart updating " + type);

                       screenManager.setScreen("Update Progress");
                       ((UpdateProgressScreen *)screenManager.getCurrentScreen())->setProgress(0);
                       display.display();
                       //
                     });

  ArduinoOTA.onEnd([]()
                   {
                     otaInProgress = false;

                     Serial.println("End. Rebooting!");

                     screenManager.setScreen("Update Progress");
                     ((UpdateProgressScreen *)screenManager.getCurrentScreen())->setMessage("Rebooting...");
                     display.display();

                     ESP.restart();
                     //
                   });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        {
                          uint8_t percent = (progress / (total / 100));

                          if (percent == lastPercent)
                            return;
                          lastPercent = percent;

                          Serial.println("Progress: " + String(percent) + "%");

                          ((UpdateProgressScreen *)screenManager.getCurrentScreen())->setProgress(percent);
                          display.display();
                          //
                        });

  ArduinoOTA.onError([](ota_error_t error)
                     {
                       otaInProgress = false;

                       if (error == OTA_AUTH_ERROR) // 0
                         Serial.println("Auth Failed");
                       else if (error == OTA_BEGIN_ERROR) // 1
                         Serial.println("Begin Failed");
                       else if (error == OTA_CONNECT_ERROR) // 2
                         Serial.println("Connect Failed");
                       else if (error == OTA_RECEIVE_ERROR) // 3
                         Serial.println("Receive Failed");
                       else if (error == OTA_END_ERROR) // 4
                         Serial.println("End Failed");

                       screenManager.setScreen("Update Progress");
                       ((UpdateProgressScreen *)screenManager.getCurrentScreen())->setMessage("Error: " + String(error));
                       display.display();

                       delay(1000);
                       //
                     });

  ArduinoOTA.begin();
  otaSetup = true;

  Serial.println("\t[INFO] [OTA] Initialized");
}