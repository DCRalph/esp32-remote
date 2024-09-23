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

unsigned long long prevMillis1;
int interval1 = 200;

bool sleepCountdown = false;
unsigned long long sleepCountdownMillis = 0;
long sleepCountdownTime = 1500;
long sleepDisplayTime = 500;


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
  

  wireless.setup();
  wireless.setRecvCb(espNowCb);

  prevMillis1 = millis();

  // for (int i = 0; i < 300; i++)
  // {
  //   BigClass bigClass;
  //   bigClass.genvalues();

  //   bigVector.push_back(bigClass);
  // }

  led.Off();
  btnLed.Off();
  screenManager.setScreen("Home");
  buttons.update();
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

  buttons.update();

  if (millis() - prevMillis1 > interval1)
  {
    prevMillis1 = millis();
    battery.update();

    // for (uint64_t i = 0; i < bigVector.size(); i++)
    // {
    //   bigVector[i].expensiceFunction();
    // }

    // Serial.println(battery.getVoltage());

    // String voltageS = (String)battery.getVoltage();

    // Serial.print("1: ");
    // Serial.print(digitalRead(1));
    // Serial.print(" 2: ");
    // Serial.println(digitalRead(2));

    // log_i("Total heap: %u", ESP.getHeapSize());
    // log_i("Free heap: %u", ESP.getFreeHeap());
    // log_i("Total PSRAM: %u", ESP.getPsramSize());
    // log_i("Free PSRAM: %d", ESP.getFreePsram());
    // log_i("spiram size %u", esp_spiram_get_size());
  }

  if (!sleepLoop())
    display.display();
}
