#include <Arduino.h>
#include "secrets.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include "ha.h"

#include "Battery.h"
#include "Buttons.h"

// #include "fonts/NotoSansBold36.h"

#include "driver/Display.h"
#include "screens/Error.h"
#include "screens/Menu.h"
#include "screens/RSSIMeter.h"
#include "screens/SettingsScreen.h"
#include "screens/Settings/batteryScreen.h"
#include "screens/Settings/wifiScreen.h"


#include "screens/desk_lamp.h"
#include "screens/haLight.h"
#include "screens/haSwitch.h"

// #define LED_PIN 38

WiFiClient espClient;
PubSubClient client(espClient);

ErrorScreen errorScreen("Error", "error");
MenuScreen menuScreen("Menu", "menu");
RSSIMeter rssiMeter("RSSI", "rssi");
Settings settings("Settings", "settings");
batteryScreen batteryScreen("Battery", "battery");
wifiScreen wifiScreen("Wi-Fi", "wifi");



// DeskLamp deskLamp(":)", "desk_lamp");

// HALight deskLamp("deskLamp", "light.midesklamp1s_9479");
// HALight leds("leds", "light.william_strip");
// HALight matrix("matrix", "light.matrix_lamp");
// HALight michaelLeds("michaelLeds", "light.micheals_leds");

// HALight livingRoom("livingRoom", "light.living_room");

// // HASwitch bedLight("bedLight", "switch.sonoffbasic_1");
// HASwitch bedLight("bedLight", "switch.sonoffmini_1");
// // HASwitch michaelFan("michaelFan", "switch.michael_plug_2");
// HASwitch michaelFan("michaelFan", "switch.sonoffmini_2");

unsigned long long prevMillis1;
int interval1 = 200;

unsigned long long prevMillis2;
int interval2 = 10000;

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
    {
      delay(5000);
    }
  }
}

void setup()
{
  // Serial.begin(115200);
  // pinMode(LED_PIN, OUTPUT);

  // setup lcd
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

  screenManager.setScreen("menu");

  //********************************************
  // {

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  //   while (WiFi.status() != WL_CONNECTED)
  //   {
  //     delay(500);
  //   }
    WiFi.setAutoReconnect(true);

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
    // display.sprite.drawString("cancel", LCD_WIDTH / 2, 130);

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
