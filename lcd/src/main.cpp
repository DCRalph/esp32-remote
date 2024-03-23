// lcd mac 74:4d:bd:7b:93:6c

#include <Arduino.h>
#include "secrets.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include "ha.h"

#include "Battery.h"
#include "Buttons.h"
#include "myespnow.h"

#include "driver/Display.h"
#include "screens/Error.h"
#include "screens/Menu.h"
#include "screens/RSSIMeter.h"
#include "screens/SettingsScreen.h"
#include "screens/Settings/batteryScreen.h"
#include "screens/Settings/wifiScreen.h"
#include "screens/EspnowScreen.h"

#include "screens/desk_lamp.h"
#include "screens/haLight.h"
#include "screens/haSwitch.h"

// #define LED_PIN 38

WiFiClient espClient;
PubSubClient client(espClient);

ErrorScreen errorScreen("Error");
MenuScreen menuScreen("Menu");
RSSIMeter rssiMeter("RSSI");
Settings settings("Settings");
BatteryScreen batteryScreen("Battery");
WifiScreen wifiScreen("Wi-Fi");
EspnowScreen espnowScreen("Espnow");

// HALight deskLamp("deskLamp", "light.midesklamp1s_9479");
// HALight leds("leds", "light.william_strip");
// HALight matrix("matrix", "light.matrix_lamp");

unsigned long long prevMillis1;
int interval1 = 200;

bool sleepCountdown = false;
unsigned long long sleepCountdownMillis = 0;
long sleepCountdownTime = 3000;
long sleepDisplayTime = 500;

void mqttConnect()
{
  while (!client.connected())
  {
    if (client.connect("ESP32-remote", MQTT_USER, MQTT_PASS))
    {
      client.publish("esp-remote/init", "Hello from ESP32");
    }
    else
      delay(5000);
  }
}

void setup()
{
  Serial.begin(115200);
  // pinMode(LED_PIN, OUTPUT);

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  // setup buttons
  buttons.setup();
  buttons.update();

  battery.init();

  display.init();

  // setup screens
  screenManager.addScreen(&errorScreen);
  screenManager.addScreen(&menuScreen);
  screenManager.addScreen(&rssiMeter);
  screenManager.addScreen(&settings);
  screenManager.addScreen(&batteryScreen);
  screenManager.addScreen(&wifiScreen);
  screenManager.addScreen(&espnowScreen);

  screenManager.setScreen("Menu");

  //********************************************
  // {

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  //   while (WiFi.status() != WL_CONNECTED)
  //   {
  //     delay(500);
  //   }
  WiFi.setAutoReconnect(true);

  myEspnow.init();

  //   client.setServer(MQTT_SERVER, MQTT_PORT);
  //   client.setBufferSize(1024);
  // }

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
    // turn off lcd
    digitalWrite(15, LOW);

    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
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
  }

  // if (millis() - prevMillis2 > interval2)
  // {
  //   prevMillis2 = millis();

  //   battery.update();

  //   char buf[10];
  //   sprintf(buf, "%f", battery.getVoltage());

  //   client.publish("esp/batt", buf);
  // }

  if (!sleepLoop())
    display.display();
}
