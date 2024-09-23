// lcd mac 74:4d:bd:7b:93:6c

#include "config.h"

#include <WiFi.h>

#include "IO/Battery.h"
#include "IO/buttons.h"
#include "IO/Wireless.h"

#include "IO/Display.h"
#include "IO/ScreenManager.h"

#include "screens/Error.h"

#include "screens/Home.h"

#include "screens/SettingsScreen.h"
#include "screens/Settings/RSSIMeter.h"
#include "screens/Settings/batteryScreen.h"
#include "screens/Settings/WiFiInfo.h"
#include "screens/Settings/SystemInfoScreen.h"  

#include "screens/Send.h"

#include "screens/Control.h"

// /control
#include "screens/Control/CarLocks.h"
#include "screens/Control/Car.h"
#include "screens/Control/CarFlash.h"

#include "IO/ML.h"

WiFiClient espClient;

ErrorScreen errorScreen("Error");
HomeScreen homeScreen("Home");

Settings settings("Settings");
RSSIMeter rssiMeter("RSSI");
BatteryScreen batteryScreen("Battery");
WiFiInfoScreen WiFiInfo("Wi-Fi info");
SystemInfoScreen systemInfoScreen("System Info");

SendScreen sendScreen("Send");

ControlScreen controlScreen("Control");

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

  display.init();

  // setup screens
  screenManager.addScreen(&errorScreen);
  screenManager.addScreen(&homeScreen);

  // /settings
  screenManager.addScreen(&rssiMeter);
  screenManager.addScreen(&settings);
  screenManager.addScreen(&batteryScreen);
  screenManager.addScreen(&WiFiInfo);
  screenManager.addScreen(&systemInfoScreen);

  screenManager.addScreen(&sendScreen);

  screenManager.addScreen(&controlScreen);

  // /control
  screenManager.addScreen(&carLocksScreen);
  screenManager.addScreen(&carScreen);
  screenManager.addScreen(&carFlashScreen);

  screenManager.setScreen("Home");

  // WiFi.mode(WIFI_STA);
  // WiFi.begin(WIFI_SSID, WIFI_PASS);
  // WiFi.setAutoReconnect(true);

  wireless.setup();

  // WiFi.mode(WIFI_STA);
  // esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  // WiFi.disconnect();
  // if (esp_now_init() == ESP_OK)
  // {
  //   Serial.println("ESPNow Init Success");
  // }
  // else
  // {
  //   Serial.println("ESPNow Init Failed");
  //   // Retry InitESPNow, add a counte and then restart?
  //   // InitESPNow();
  //   // or Simply Restart
  //   ESP.restart();
  // }

  // memcpy(peerInfo.peer_addr, peer_addr, 6);
  // peerInfo.channel = 1;
  // peerInfo.encrypt = false;
  // // peerInfo.ifidx = WIFI_IF_STA;

  // if (esp_now_add_peer(&peerInfo) != ESP_OK)
  // {
  //   Serial.println("Failed to add peer");
  // }

  // esp_now_register_send_cb(OnDataSent);

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

    display.sprite.setTextSize(2);
    display.sprite.setTextDatum(TC_DATUM);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString("Sleeping...", LCD_WIDTH / 2, 60);
    display.sprite.drawString("Release to cancel", LCD_WIDTH / 2, 110);

    char buf[20];
    long msToGo = sleepCountdownTime - (millis() - sleepCountdownMillis);

    sprintf(buf, "in %.1f S", float(msToGo) / 1000);
    display.sprite.drawString(buf, LCD_WIDTH / 2, 80);

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
    digitalWrite(15, LOW);
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

    // Serial.println(battery.getVoltage());

    String voltageS = (String)battery.getVoltage();
  }

  if (ClickButtonUP.clicks == 5){
    ml.init();
  }

  if (!sleepLoop())
    display.display();
}
