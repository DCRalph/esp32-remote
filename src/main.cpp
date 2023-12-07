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

#define LED_PIN 38

WiFiClient espClient;
PubSubClient client(espClient);

ErrorScreen errorScreen("Error", "error");
MenuScreen menuScreen("Menu", "menu");
RSSIMeter rssiMeter("RSSI", "rssi");
DeskLamp deskLamp(":)", "desk_lamp");

unsigned long long prevMillis1;
int interval1 = 200;

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
  pinMode(LED_PIN, OUTPUT);
  battery.init();

  ClickButton21.Update();
  ClickButton0.Update();

  display.init();
  // screenManager.init(&display);

  screenManager.addScreen(&errorScreen);
  screenManager.addScreen(&menuScreen);
  screenManager.addScreen(&rssiMeter);
  screenManager.addScreen(&deskLamp);

  screenManager.setScreen("menu");

  //********************************************
  {
    bool setupDebug = ClickButton21.depressed;

    display.sprite.fillScreen(TFT_BLACK);

    display.sprite.setTextSize(3);
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

      display.sprite.setTextSize(3);
      display.sprite.setTextDatum(MC_DATUM);
      display.sprite.setTextColor(TFT_WHITE);
      display.sprite.drawString("WiFi Info", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 45);

      display.sprite.setTextSize(2);

      display.sprite.drawString(WiFi.SSID(), LCD_WIDTH / 2, LCD_HEIGHT / 2 - 15);
      display.sprite.drawString(WiFi.localIP().toString(), LCD_WIDTH / 2, LCD_HEIGHT / 2 + 15);
      display.sprite.drawString(WiFi.macAddress(), LCD_WIDTH / 2, LCD_HEIGHT / 2 + 45);

      display.push();

      while (ClickButton21.clicks != 1)
        ClickButton21.Update();
      ClickButton21.Update();
    }

    display.sprite.fillScreen(TFT_BLACK);

    display.sprite.setTextSize(3);
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

      display.sprite.setTextSize(3);
      display.sprite.setTextDatum(MC_DATUM);
      display.sprite.setTextColor(TFT_WHITE);
      display.sprite.drawString("MQTT Info", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 45);

      display.sprite.setTextSize(2);

      display.sprite.drawString(MQTT_SERVER, LCD_WIDTH / 2, LCD_HEIGHT / 2 - 15);
      display.sprite.drawString(String(MQTT_PORT), LCD_WIDTH / 2, LCD_HEIGHT / 2 + 15);
      display.sprite.drawString(MQTT_USER, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 45);

      display.push();

      while (ClickButton21.clicks != 1)
        ClickButton21.Update();
      ClickButton21.Update();
    }

    // ha.begin(&espClient);

    display.sprite.fillScreen(TFT_BLACK);
    display.sprite.setTextSize(3);
    display.sprite.drawString("Done", LCD_WIDTH / 2, LCD_HEIGHT / 2);

    display.push();
  }

  delay(500);

  prevMillis1 = millis();
}

void loop()
{
  if (!client.connected())
    mqttConnect();
  client.loop();

  ClickButton21.Update();
  ClickButton0.Update();

  if (millis() - prevMillis1 > interval1)
    battery.update();

  if (ClickButton21.clicks == -2)
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));

  if (ClickButton0.clicks > 1 || ClickButton0.clicks < 0)
  {
    char buf[10];
    sprintf(buf, "%d", ClickButton0.clicks);
    client.publish("esp-remote/btn0", buf);
  }

  ///////////////////////////

  display.display();
}
