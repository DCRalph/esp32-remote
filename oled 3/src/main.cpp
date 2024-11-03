// lcd mac 74:4d:bd:7b:93:6c

#include "config.h"

#include <vector>

#include "IO/Battery.h"
#include "IO/buttons.h"
#include "IO/Wireless.h"

#include "IO/Display.h"
#include "IO/ScreenManager.h"

#include "screens/Error.h"

// /
#include "screens/Home.h"
#include "screens/Control.h"

// /settings
#include "screens/SettingsScreen.h"
#include "screens/Settings/RSSIMeter.h"
#include "screens/Settings/batteryScreen.h"
#include "screens/Settings/WiFiInfo.h"
#include "screens/Settings/SystemInfoScreen.h"

// /control
#include "screens/Send.h"
#include "screens/Control/CarLocks.h"
#include "screens/Control/Car.h"
#include "screens/Control/CarFlash.h"

// /remoteRelay
#include "screens/RemoteRelay.h"

#include "IO/GPIO.h"

WiFiClient espClient;

ErrorScreen errorScreen("Error");

// /
HomeScreen homeScreen("Home");
ControlScreen controlScreen("Control");

// /settings
Settings settings("Settings");
RSSIMeter rssiMeter("RSSI");
BatteryScreen batteryScreen("Battery");
WiFiInfoScreen WiFiInfo("Wi-Fi info");
SystemInfoScreen systemInfoScreen("System Info");

// /control
CarLocksScreen carLocksScreen("CarLocks");
CarControlScreen carScreen("Car");
CarFlashScreen carFlashScreen("CarFlash");

// /remoteRelay
RemoteRelayScreen remoteRelayScreen("RemoteRelay");

unsigned long long prevMillis1;
int interval1 = 200;

bool sleepCountdown = false;
unsigned long long sleepCountdownMillis = 0;
long sleepCountdownTime = 1500;
long sleepDisplayTime = 500;

unsigned long long lastDraw = 0;

TaskHandle_t fpsTask;

void fpsTaskFunction(void *pvParameters)
{
  for (;;)
  {
    lastFps = fps;
    fps = 0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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

void setup()
{
  initConfig();
  configureDeepSleep();

  GpIO::initIO();
  led.On();
  buttons.setup();

  deepSleepSetup();

  battery.init();
  battery.update();

  display.init();

  // setup screens
  screenManager.addScreen(&errorScreen);

  // /
  screenManager.addScreen(&homeScreen);
  screenManager.addScreen(&controlScreen);

  // /settings
  screenManager.addScreen(&rssiMeter);
  screenManager.addScreen(&settings);
  screenManager.addScreen(&batteryScreen);
  screenManager.addScreen(&WiFiInfo);
  screenManager.addScreen(&systemInfoScreen);

  // /control
  screenManager.addScreen(&carLocksScreen);
  screenManager.addScreen(&carScreen);
  screenManager.addScreen(&carFlashScreen);

  // /remoteRelay
  screenManager.addScreen(&remoteRelayScreen);

  wireless.setup();
  wireless.setRecvCb(espNowCb);

  prevMillis1 = millis();

  led.Off();
  btnLed.Off();
  screenManager.setScreen("Home");
  buttons.update();

  xTaskCreatePinnedToCore(
      fpsTaskFunction, /* Task function. */
      "fpsTask",       /* name of task. */
      10000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      1,               /* priority of the task */
      &fpsTask,        /* Task handle to keep track of created task */
      0);              /* pin task to core 0 */
}

bool sleepLoop()
{

  if (ClickButtonUP.depressed)
  {
    if (!sleepCountdown)
      sleepCountdownMillis = millis();
    sleepCountdown = true;
  }
  else
  {
    sleepCountdown = false;

    return false;
  }

  if (sleepCountdown && millis() - sleepCountdownMillis > sleepDisplayTime)
  {
    display.clearScreen();

    display.sprite.setTextSize(4);
    display.sprite.setTextDatum(TC_DATUM);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString("Sleeping...", LCD_WIDTH / 2, 60);
    display.sprite.drawString("Release to cancel", LCD_WIDTH / 2, 160);

    char buf[20];
    long msToGo = sleepCountdownTime - (millis() - sleepCountdownMillis);

    if (msToGo > 100)
      sprintf(buf, "in %.1f S", float(msToGo) / 1000);
    else
      sprintf(buf, "Now!");
    display.sprite.drawString(buf, LCD_WIDTH / 2, 100);

    u8_t percent = map(msToGo, 0, sleepCountdownTime - sleepDisplayTime, 0, 100);
    int x = map(percent, 0, 100, 0, LCD_WIDTH - 44);

    u8_t r = map(percent, 0, 100, 255, 100);
    u8_t g = map(percent, 0, 100, 100, 255);
    u8_t b = 0;

    u16_t color = display.tft.color565(r, g, b);

    display.sprite.drawRoundRect(20, 5, LCD_WIDTH - 40, 50, 20, TFT_WHITE);
    display.sprite.fillRoundRect(22, 6, x, 48, 20, color);

    display.push();
  }

  if (sleepCountdown && millis() - sleepCountdownMillis > sleepCountdownTime)
  {
    do
    {
      buttons.update();
    } while (ClickButtonUP.depressed || ClickButtonDOWN.depressed);
    esp_deep_sleep_start();
  }

  return sleepCountdown && millis() - sleepCountdownMillis > sleepDisplayTime;
}

void loop()
{
  fps++;

  if (millis() - prevMillis1 > interval1)
  {
    prevMillis1 = millis();
    battery.update();
  }

  if (!sleepLoop() && millis() - lastDraw > 25)
  {
    lastDraw = millis();

    buttons.update();

    frameTime = millis();
    display.display();
    lastFrameTime = millis() - frameTime;
  }
}
