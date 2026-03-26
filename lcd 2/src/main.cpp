// lcd mac 74:4d:bd:7b:93:6c

#include "config.h"

#include <WiFi.h>

#include "IO/Battery.h"
#include "IO/Buttons.h"
#include "Wireless.h"
#include "Mesh.h"

#include "Display.h"
#include "MenuInput.h"
#include "TFTDisplayDriver.h"
#include "ScreenManager.h"

#include "screens/Home.h"

WiFiClient espClient;

TFTDisplayDriverContext displayDriverContext;

int getBatteryPercentage()
{
  return battery.getPercentageI();
}

unsigned long long prevMillis1;
int interval1 = 200;

bool sleepCountdown = false;
unsigned long long sleepCountdownMillis = 0;
long sleepCountdownTime = 1500;
long sleepDisplayTime = 500;

void setup()
{
  initConfig();
  // pinMode(LED_PIN, OUTPUT);

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  while (!digitalRead(BTN_DOWN_PIN) || !digitalRead(BTN_UP_PIN))
  {
  }

  // setup buttons
  buttons.setup();
  buttons.update();

  battery.init();

  const DisplayConfig displayConfig = TFTDisplayDriver::makeConfig(
      &displayDriverContext,
      LCD_WIDTH,
      LCD_HEIGHT,
      TFT_ROT,
      16);
  display.begin(displayConfig, &screenManager);
  screenManager.init(display);

  MenuInputConfig menuInputConfig;
  menuInputConfig.mode = MenuInputMode::Buttons2;
  menuInputConfig.navigationClicks = 1;   // 1 click = up/down
  menuInputConfig.defaultSelectClicks = 2; // 2 clicks = default action (e.g. select / run default route)
  menuInputConfig.getUpClicks = []() { return (int)ClickButtonUP.clicks; };
  menuInputConfig.getDownClicks = []() { return (int)ClickButtonDOWN.clicks; };
  MenuInput::configure(menuInputConfig);

  screenManager.setScreen(&HomeScreen);

  auto syncMgr = SyncManager::getInstance();

  syncMgr->setTransport(&wireless);

  syncMgr->setDeviceIdProvider([]() -> uint32_t
                               {
                                 return 0x67;
                               });

  syncMgr->setModePersistence(
      []() -> uint8_t
      {
        if (!preferences.isKey("meshSyncMode"))
          return static_cast<uint8_t>(SyncMode::JOIN);
        return preferences.getUChar("meshSyncMode", static_cast<uint8_t>(SyncMode::JOIN));
      },
      [](uint8_t m)
      {
        preferences.putUChar("meshSyncMode", m);
      });

  syncMgr->begin();

  prevMillis1 = millis();
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

    display.setTextSize(2);
    display.setTextDatum(TC_DATUM);
    display.setTextColor(TFT_WHITE);
    display.drawString("Sleeping...", LCD_WIDTH / 2, 60);
    display.drawString("Release to cancel", LCD_WIDTH / 2, 110);

    char buf[20];
    long msToGo = sleepCountdownTime - (millis() - sleepCountdownMillis);

    sprintf(buf, "in %.1f S", float(msToGo) / 1000);
    display.drawString(buf, LCD_WIDTH / 2, 80);

    u8_t percent = map(msToGo, 0, sleepCountdownTime - sleepDisplayTime, 0, 100);
    int x = map(percent, 0, 100, 0, LCD_WIDTH - 44);

    u8_t r = map(percent, 0, 100, 255, 100);
    u8_t g = map(percent, 0, 100, 100, 255);
    u8_t b = 0;

    u16_t color = display.color565(r, g, b);

    display.drawRoundRect(20, 5, LCD_WIDTH - 40, 50, 20, TFT_WHITE);
    display.fillRoundRect(22, 6, x, 48, 20, color);

    display.push();
  }

  if (sleepCountdown && millis() - sleepCountdownMillis > sleepCountdownTime)
  {
    digitalWrite(15, LOW);
    esp_deep_sleep_start();
  }

  return sleepCountdown && millis() - sleepCountdownMillis > sleepDisplayTime;
}

void loop()
{


  buttons.update();
  SyncManager::getInstance()->loop();

  if (millis() - prevMillis1 > interval1)
  {
    prevMillis1 = millis();
    battery.update();

    String voltageS = (String)battery.getVoltage();
  }

  if (!sleepLoop())
    display.render();
}
