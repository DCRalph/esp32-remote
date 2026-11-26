// lcd mac 74:4d:bd:7b:93:6c

#include "config.h"

#include <vector>

#include "IO/Battery.h"
#include "IO/Buttons.h"
#include "Wireless.h"
#include <Mesh.h>

#include <dcr_Console.h>
#include <dcr_FatalHandler.h>
#include <dcr_Logger.h>
#include <dcr_TaskManager.h>
#include <dcr_TimeProfiler.h>
#include <dcr_display.h>

#include "IO/AmoledDisplayDriver.h"

#include "screens/Home.h"

#include "IO/GPIO.h"

WiFiClient espClient;
AmoledDisplayDriver displayDriver(LCD_WIDTH, LCD_HEIGHT, TFT_ROT);

// The two buttons sit left (btnUp) and right (btnDown), so they step like an encoder:
// left moves up a menu and decreases a value being edited, right the opposite.
// A double click on either selects, the trigger selects too.
EncoderInput buttonInput([]
                         {
  EncoderInput::Config c;
  c.readDelta = []() -> int32_t { return (btnDown.clicks() == 1) - (btnUp.clicks() == 1); };
  c.readClicks = []()
  {
    if (btnUp.clicks() > 1)
      return btnUp.clicks();
    if (btnDown.clicks() > 1)
      return btnDown.clicks();
    return btnTrigger.clicks();
  };
  c.defaultSelectClicks = 2;
  c.accelerate = false;
  return c; }());

unsigned long long prevMillis1;
int interval1 = 500;

bool sleepCountdown = false;
unsigned long long sleepCountdownMillis = 0;
long sleepCountdownTime = 1500;
long sleepDisplayTime = 500;


void setup()
{
  initConfig();
  configureDeepSleep();

  Serial.begin(115200);
  Console::addStream(&Serial);
  Logger::SetLevel(LogLevel::Debug);
  Logger::InstallLogHook();
  FatalHandler::install();
  taskManager.begin();
  timeProfiler.begin(2048, 1, 0);

  GpIO::initIO();
  led.On();
  buttons.setup();

  deepSleepSetup();

  battery.init();
  battery.update();

  display.begin(displayDriver);
  display.setBatteryProvider([]()
                             { return (int)battery.getPercentageI(); });
  Inputs::addSource(buttonInput);

  screenManager.setScreen(&HomeScreen);

  auto syncMgr = SyncManager::getInstance();

  syncMgr->setTransport(&wireless);
  syncMgr->setDeviceIdProvider([]() -> uint32_t
                               { return 0x69; });

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
  RemoteProtocol::attachDirect();

  prevMillis1 = millis();

  led.Off();
  btnLed.Off();
  buttons.update();
}

bool sleepLoop()
{

  if (btnUp.pressed())
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

    display.setTextSize(4);
    display.setTextDatum(TC_DATUM);
    display.setTextColor(TFT_WHITE);
    display.drawString("Sleeping...", LCD_WIDTH / 2, 60);
    display.drawString("Release to cancel", LCD_WIDTH / 2, 160);

    char buf[20];
    long msToGo = sleepCountdownTime - (millis() - sleepCountdownMillis);

    if (msToGo > 100)
      sprintf(buf, "in %.1f S", float(msToGo) / 1000);
    else
      sprintf(buf, "Now!");
    display.drawString(buf, LCD_WIDTH / 2, 100);

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
    do
    {
      buttons.update();
    } while (btnUp.pressed() || btnDown.pressed());
    esp_deep_sleep_start();
  }

  return sleepCountdown && millis() - sleepCountdownMillis > sleepDisplayTime;
}

void loop()
{
  const uint32_t frameStartMs = millis();
  DirectComms::getInstance()->loop(); // expire RPCs before a late reply is drained
  SyncManager::getInstance()->loop();

  if (millis() - prevMillis1 > interval1)
  {
    prevMillis1 = millis();
    battery.update();
    debugD("Battery: %fV", battery.getVoltage());
  }

  buttons.update();
  if (!sleepLoop())
  {
    timeProfiler.increment(PROFILE_FRAME);
    timeProfiler.start(PROFILE_FRAME);
    display.render();
    timeProfiler.stop(PROFILE_FRAME);
  }

  // Sleep out the rest of the frame rather than spinning (no catch-up bursts).
  const uint32_t spentMs = millis() - frameStartMs;
  if (spentMs < FRAME_INTERVAL_MS)
    vTaskDelay(pdMS_TO_TICKS(FRAME_INTERVAL_MS - spentMs));
}
