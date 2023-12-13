#include <Arduino.h>
#include "secrets.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include "ha.h"

#include "battery.h"
#include "buttons.h"

// #include "fonts/NotoSansBold36.h"

#include "driver/Display.h"
#include "screens/Error.h"
#include "screens/Menu.h"
#include "screens/RSSIMeter.h"
#include "screens/desk_lamp.h"
#include "screens/haLight.h"
#include "screens/haSwitch.h"

// #define LED_PIN 38

WiFiClient espClient;
PubSubClient client(espClient);

ErrorScreen errorScreen("Error", "error");
MenuScreen menuScreen("Menu", "menu");
RSSIMeter rssiMeter("RSSI", "rssi");
// DeskLamp deskLamp(":)", "desk_lamp");

HALight deskLamp("deskLamp", "light.midesklamp1s_9479");
HALight leds("leds", "light.william_strip");
HALight matrix("matrix", "light.matrix_lamp");
HALight michaelLeds("michaelLeds", "light.micheals_leds");

HALight livingRoom("livingRoom", "light.living_room");

HASwitch bedLight("bedLight", "switch.sonoffbasic_1");
HASwitch michaelFan("michaelFan", "switch.michael_plug_2");

unsigned long long prevMillis1;
int interval1 = 200;

unsigned long long prevMillis2;
int interval2 = 10000;

bool sleepCountdown = false;
unsigned long long sleepCountdownMillis = 0;
long sleepCountdownTime = 3500;

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
      // Serial.print("failed, rc=");
      // Serial.print(client.state());
      // Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  // Serial.begin(115200);
  // pinMode(LED_PIN, OUTPUT);

  // turn on lcd
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  battery.init();

  ClickButton1.Update();
  ClickButton0.Update();

  display.init();
  // screenManager.init(&display);

  screenManager.addScreen(&errorScreen);
  screenManager.addScreen(&menuScreen);
  screenManager.addScreen(&rssiMeter);
  screenManager.addScreen(&deskLamp);
  screenManager.addScreen(&leds);
  screenManager.addScreen(&matrix);
  screenManager.addScreen(&livingRoom);
  screenManager.addScreen(&bedLight);
  screenManager.addScreen(&michaelFan);

  screenManager.addScreen(&michaelLeds);

  screenManager.setScreen("menu");

  //********************************************
  {
    bool setupDebug = ClickButton1.depressed;

    display.sprite.fillScreen(TFT_BLACK);

    display.sprite.setTextSize(2);
    display.sprite.setTextDatum(MC_DATUM);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString("Loading...", LCD_WIDTH / 2, LCD_HEIGHT / 2);
    display.sprite.drawString("WiFi...", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);

    if (setupDebug)
    {
      display.sprite.setTextSize(4);
      display.sprite.setTextDatum(MC_DATUM);
      display.sprite.setTextColor(TFT_SKYBLUE);
      display.sprite.drawString("DEBUG", LCD_WIDTH / 2, 50);
    }

    display.push();

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
    }
    WiFi.setAutoReconnect(true);

    if (setupDebug)
    {
      display.sprite.fillScreen(TFT_BLACK);

      display.sprite.setTextSize(4);
      display.sprite.setTextDatum(MC_DATUM);
      display.sprite.setTextColor(TFT_SKYBLUE);
      display.sprite.drawString("DEBUG", LCD_WIDTH / 2, 50);

      display.sprite.setTextSize(2);
      display.sprite.setTextDatum(MC_DATUM);
      display.sprite.setTextColor(TFT_WHITE);
      display.sprite.drawString("WiFi Info", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 45);

      display.sprite.setTextSize(1);

      display.sprite.drawString(WiFi.SSID(), LCD_WIDTH / 2, LCD_HEIGHT / 2 - 15);
      display.sprite.drawString(WiFi.localIP().toString(), LCD_WIDTH / 2, LCD_HEIGHT / 2 + 15);
      display.sprite.drawString(WiFi.macAddress(), LCD_WIDTH / 2, LCD_HEIGHT / 2 + 45);

      display.push();

      while (ClickButton1.clicks != 1)
        ClickButton1.Update();
      ClickButton1.Update();
    }

    display.sprite.fillScreen(TFT_BLACK);

    display.sprite.setTextSize(2);
    display.sprite.setTextDatum(MC_DATUM);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString("Loading...", LCD_WIDTH / 2, LCD_HEIGHT / 2);
    display.sprite.drawString("MQTT...", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);

    if (setupDebug)
    {
      display.sprite.setTextSize(4);
      display.sprite.setTextDatum(MC_DATUM);
      display.sprite.setTextColor(TFT_SKYBLUE);
      display.sprite.drawString("DEBUG", LCD_WIDTH / 2, 50);
    }

    display.push();

    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setBufferSize(1024);

    mqttConnect();
    client.loop();

    if (setupDebug)
    {
      display.sprite.fillScreen(TFT_BLACK);

      display.sprite.setTextSize(4);
      display.sprite.setTextDatum(MC_DATUM);
      display.sprite.setTextColor(TFT_SKYBLUE);
      display.sprite.drawString("DEBUG", LCD_WIDTH / 2, 50);

      display.sprite.setTextSize(2);
      display.sprite.setTextDatum(MC_DATUM);
      display.sprite.setTextColor(TFT_WHITE);
      display.sprite.drawString("MQTT Info", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 45);

      display.sprite.setTextSize(1);

      display.sprite.drawString(MQTT_SERVER, LCD_WIDTH / 2, LCD_HEIGHT / 2 - 15);
      display.sprite.drawString(String(MQTT_PORT), LCD_WIDTH / 2, LCD_HEIGHT / 2 + 15);
      display.sprite.drawString(MQTT_USER, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 45);

      display.push();

      while (ClickButton1.clicks != 1)
        ClickButton1.Update();
      ClickButton1.Update();
    }

    display.sprite.fillScreen(TFT_BLACK);

    display.sprite.setTextSize(2);
    display.sprite.setTextDatum(MC_DATUM);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString("Loading...", LCD_WIDTH / 2, LCD_HEIGHT / 2);
    display.sprite.drawString("Screens..", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);

    if (setupDebug)
    {
      display.sprite.setTextSize(4);
      display.sprite.setTextDatum(MC_DATUM);
      display.sprite.setTextColor(TFT_SKYBLUE);
      display.sprite.drawString("DEBUG", LCD_WIDTH / 2, 50);
    }

    display.push();
  }

  deskLamp.init();
  leds.init();
  matrix.init();
  bedLight.init();
  livingRoom.init();

  michaelLeds.init();
  michaelFan.init();

  // ha.begin(&espClient);

  display.sprite.fillScreen(TFT_BLACK);
  display.sprite.setTextSize(3);
  display.sprite.drawString("Done", LCD_WIDTH / 2, LCD_HEIGHT / 2);

  display.push();

  delay(500);

  prevMillis1 = millis();
}

void loop()
{
  if (!client.connected())
    mqttConnect();
  client.loop();

  ClickButton1.Update();
  ClickButton0.Update();

  if (millis() - prevMillis1 > interval1)
  {
    prevMillis1 = millis();
    battery.update();
  }

  if (millis() - prevMillis2 > interval2)
  {
    prevMillis2 = millis();

    battery.update();

    char buf[10];
    sprintf(buf, "%f", battery.getVoltage());

    client.publish("esp/batt", buf);
  }

  // if (ClickButton1.clicks == -2)
  //   digitalWrite(LED_PIN, !digitalRead(LED_PIN));

  if (ClickButton0.clicks > 1 || ClickButton0.clicks < 0)
  {
    char buf[10];
    sprintf(buf, "%d", ClickButton0.clicks);
    client.publish("esp-remote/btn0", buf);
  }

  if (ClickButton1.depressed)
  {
    if (!sleepCountdown)
      sleepCountdownMillis = millis();
    sleepCountdown = true;
  }
  else
  {
    sleepCountdown = false;
  }

  if (sleepCountdown && millis() - sleepCountdownMillis > sleepCountdownTime)
  {
    // turn off lcd
    digitalWrite(15, LOW);

    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
    esp_deep_sleep_start();
  }

  ///////////////////////////
  if (sleepCountdown && millis() - sleepCountdownMillis > 1000)
  {
    display.sprite.fillScreen(TFT_BLACK);

    display.sprite.setTextSize(2);
    display.sprite.setTextDatum(MC_DATUM);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString("Sleeping...", LCD_WIDTH / 2, LCD_HEIGHT / 2);
    display.sprite.drawString("Release to", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 60);
    display.sprite.drawString("cancel", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 80);

    char buf[20];
    long msToGo = sleepCountdownTime - (millis() - sleepCountdownMillis);

    sprintf(buf, "in %.1f S", float(msToGo) / 1000);
    display.sprite.drawString(buf, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);

    u16_t color = TFT_WHITE;
    int x = map(msToGo, 0, sleepCountdownTime, 0, LCD_WIDTH - 44);

    display.sprite.drawRoundRect(20, 50, LCD_WIDTH - 40, 50, 20, TFT_WHITE);
    display.sprite.fillRoundRect(22, 52, x, 46, 20, display.tft.color565(255, 0, 0));

    display.push();
  }
  else
  {
    display.display();
  }
}
