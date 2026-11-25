#include "InitOTA.h"

#include <Display.h>
#include <ScreenManager.h>

#include "Screens/Screens.h"
#include "Screens/UpdateProgress.h"

#undef LOG_TAG
#define LOG_TAG "OTA"

bool otaSetup = false;
bool otaInProgress = false;

static int lastPercent = 0;

/**
 * Switch to the progress screen and paint it now. `setScreen` only queues the
 * change, and OTA callbacks never return to the main loop, so the change has to
 * be applied by hand before rendering.
 */
static void showProgressScreen()
{
  screenManager.setScreen(&UpdateProgressScreen2);
  screenManager.applyPendingScreenChange();
  display.render();
}

void InitOta()
{
  debugI("Initializing...");

  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.onStart([]()
                     {
                       otaInProgress = true;

                       const char *type = (ArduinoOTA.getCommand() == U_FLASH) ? "Sketch" : "Filesystem";
                       debugI("Start updating %s", type);

                       UpdateProgress::setProgress(0);
                       showProgressScreen();
                       //
                     });

  ArduinoOTA.onEnd([]()
                   {
                     otaInProgress = false;

                     debugI("End. Rebooting!");

                     UpdateProgress::setMessage("Rebooting...");
                     showProgressScreen();

                     ESP.restart();
                     //
                   });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        {
                          uint8_t percent = (progress / (total / 100));

                          if (percent == lastPercent)
                            return;
                          lastPercent = percent;

                          debugD("Progress: %u%%", percent);

                          UpdateProgress::setProgress(percent);
                          display.render();
                          //
                        });

  ArduinoOTA.onError([](ota_error_t error)
                     {
                       otaInProgress = false;

                       switch (error)
                       {
                       case OTA_AUTH_ERROR:
                         debugE("Auth Failed");
                         break;
                       case OTA_BEGIN_ERROR:
                         debugE("Begin Failed");
                         break;
                       case OTA_CONNECT_ERROR:
                         debugE("Connect Failed");
                         break;
                       case OTA_RECEIVE_ERROR:
                         debugE("Receive Failed");
                         break;
                       case OTA_END_ERROR:
                         debugE("End Failed");
                         break;
                       default:
                         debugE("Unknown error %d", error);
                         break;
                       }

                       UpdateProgress::setMessage("Error: " + String(error));
                       showProgressScreen();

                       delay(1000);
                       //
                     });

  ArduinoOTA.begin();
  otaSetup = true;

  debugI("Initialized");
}
