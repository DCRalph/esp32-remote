// lcd mac 74:4d:bd:7b:93:6c

#include "config.h"

#include <vector>

#include "IO/Battery.h"
#include "IO/Buttons.h"
#include "Wireless.h"
#include <Mesh.h>

#include <Display.h>
#include <ScreenManager.h>
#include "IO/AmoledDisplayDriver.h"
#include "MenuInput.h"

#include "screens/Home.h"

#include "IO/GPIO.h"

WiFiClient espClient;
AmoledDisplayDriverContext displayDriverContext;

int getBatteryPercentage()
{
  return battery.getPercentageI();
}

unsigned long long prevMillis1;
int interval1 = 500;

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

void onWirelessRecv(const TransportAddress &, const TransportPacket &pkt)
{
  if (pkt.type == CMD_RELAY_1_GET)
    globalRelay1 = pkt.data[0];
  else if (pkt.type == CMD_RELAY_2_GET)
    globalRelay2 = pkt.data[0];
  else if (pkt.type == CMD_RELAY_3_GET)
    globalRelay3 = pkt.data[0];
  else if (pkt.type == CMD_RELAY_4_GET)
    globalRelay4 = pkt.data[0];
  else if (pkt.type == CMD_RELAY_5_GET)
    globalRelay5 = pkt.data[0];
  else if (pkt.type == CMD_RELAY_6_GET)
    globalRelay6 = pkt.data[0];

  else if (pkt.type == CMD_RELAY_ALL)
  {
    globalRelay1 = pkt.data[0];
    globalRelay2 = pkt.data[1];
    globalRelay3 = pkt.data[2];
    globalRelay4 = pkt.data[3];
    globalRelay5 = pkt.data[4];
    globalRelay6 = pkt.data[5];
  }

}

void setup()
{
  initConfig();
  configureDeepSleep();

  Serial.begin(115200);

  GpIO::initIO();
  led.On();
  buttons.setup();

  deepSleepSetup();

  battery.init();
  battery.update();

  DisplayConfig displayConfig = AmoledDisplayDriver::makeConfig(&displayDriverContext, LCD_WIDTH, LCD_HEIGHT, TFT_ROT, 16);
  display.begin(displayConfig, &screenManager);
  screenManager.init(display);

  MenuInputConfig menuInputConfig;
  menuInputConfig.mode = MenuInputMode::Buttons2;
  menuInputConfig.navigationClicks = 1;
  menuInputConfig.defaultSelectClicks = 2;
  menuInputConfig.getUpClicks = []()
  { return (int)ClickButtonUP.clicks; };
  menuInputConfig.getDownClicks = []()
  { return (int)ClickButtonDOWN.clicks; };
  menuInputConfig.getSelectClicks = []()
  { return (int)ClickButtonTRIGGER.clicks; };
  MenuInput::configure(menuInputConfig);

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

  prevMillis1 = millis();

  led.Off();
  btnLed.Off();
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
    } while (ClickButtonUP.depressed || ClickButtonDOWN.depressed);
    esp_deep_sleep_start();
  }

  return sleepCountdown && millis() - sleepCountdownMillis > sleepDisplayTime;
}

void loop()
{
  fps++;

  SyncManager::getInstance()->loop();

  if (millis() - prevMillis1 > interval1)
  {
    prevMillis1 = millis();
    battery.update();
    ESP_LOGI("Loop", "Battery: %fV", battery.getVoltage());
  }

  if (!sleepLoop() && millis() - lastDraw > (1000 / 30))
  {
    lastDraw = millis();

    buttons.update();

    frameTime = millis();
    display.render();
    lastFrameTime = millis() - frameTime;
  }
}
