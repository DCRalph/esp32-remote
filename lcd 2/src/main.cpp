// lcd mac 74:4d:bd:7b:93:6c

#include "config.h"

#include <WiFi.h>
#include <dcr_HttpRequest.h>
#include <dcr_NetLink.h>
#include <dcr_logger.h>
#include <dcr_console.h>
#include <dcr_fatalHandler.h>
#include <dcr_files.h>
#include <dcr_taskManager.h>

#include "IO/Battery.h"
#include "IO/Buttons.h"
#include "Wireless.h"
#include "Mesh.h"

#include <TFTDisplayDriver.h>
#include <dcr_display.h>

#include "screens/Home.h"
#include "companion/CompanionHttp.h"
#include "companion/CompanionRadio.h"

#if __has_include("secrets.h")
#include "secrets.h"
#endif

TFTDisplayDriver displayDriver(LCD_WIDTH, LCD_HEIGHT, TFT_ROT);

// Two buttons: one click moves, a double click selects.
ButtonInput buttonInput([]
                        {
  ButtonInput::Config c;
  c.up = []() { return (int)ClickButtonUP.clicks; };
  c.down = []() { return (int)ClickButtonDOWN.clicks; };
  c.defaultSelectClicks = 2;
  c.multiClickSelects = true;
  return c; }());

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

  Serial.begin(115200); // usb serial
  Serial.setTimeout(10);

  Console::addStream(&Serial);

  Files::init(); // <files/Files.h>
  // Logger::SetLevel(LogLevel::Trace);
  Logger::SetLevel(LogLevel::Debug);
  // Logger::SetLevel(LogLevel::Info);
  Logger::InstallLogHook();
  FatalHandler::install();
  taskManager.begin();

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  while (!digitalRead(BTN_DOWN_PIN) || !digitalRead(BTN_UP_PIN))
  {
  }

  // setup buttons
  buttons.setup();
  buttons.update();

  battery.init();

  display.begin(displayDriver);
  display.setBatteryProvider([]()
                             { return (int)battery.getPercentageI(); });
  Inputs::addSource(buttonInput);

  screenManager.setScreen(&HomeScreen);

  auto syncMgr = SyncManager::getInstance();

  syncMgr->setTransport(Wireless::getInstance());

  syncMgr->setDeviceIdProvider([]() -> uint32_t
                               { return 0x67; });

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

  NetLinkCallbacks netCallbacks;
  netCallbacks.isBleActive = []()
  { return false; };
  netCallbacks.isCellularPreferred = []()
  { return false; };
  netCallbacks.currentUnixTime = [](bool &timeOk)
  {
    timeOk = false;
    return (uint32_t)(millis() / 1000);
  };
  netLink.setCallbacks(netCallbacks);

#ifdef FACTORY_WIFI_SSID
  netLink.setFactoryCredentials(FACTORY_WIFI_SSID, FACTORY_WIFI_PASSWORD);
#endif

  netLink.init();
  HTTP::init();
  HTTP::setUserAgent("ProlinkDisplay/1.0");

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
  netLink.loop();
  if (CompanionHttp::isActive())
    CompanionHttp::loop();
  if (!CompanionRadio::isMeshSuspended())
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
