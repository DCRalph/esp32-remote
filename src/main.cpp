#include <Arduino.h>
#include "secrets.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include "ha.h"

#include "battery.h"
#include "buttons.h"

#include <TFT_eSPI.h>
#include "driver/rm67162.h"
// #include "fonts/NotoSansBold36.h"

#include "driver/Screen.h"
#include "screens/Error.h"
#include "screens/Menu.h"
#include "screens/RSSIMeter.h"
#include "screens/desk_lamp.h"

#define LED_PIN 38

WiFiClient espClient;
PubSubClient client(espClient);

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

ErrorScreen errorScreen(&sprite, "Error");
MenuScreen menuScreen(&sprite, "Menu");
RSSIMeter rssiMeter(&sprite, "RSSI");
DeskLamp deskLamp(&sprite, ":)");

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

void Push2TFT()
{
  lcd_PushColors(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint16_t *)sprite.getPointer());
}

void setup()
{
  // Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  sprite.createSprite(LCD_WIDTH, LCD_HEIGHT);
  sprite.setSwapBytes(1);

  rm67162_init(); // amoled lcd initialization
  lcd_setRotation(TFT_ROT);
  lcd_brightness(255);

  ClickButton0.Update();
  ClickButton21.Update();

  bool setupDebug = ClickButton21.depressed;

  sprite.fillScreen(TFT_BLACK);

  sprite.setTextSize(3);
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("Loading...", LCD_WIDTH / 2, LCD_HEIGHT / 2);
  sprite.drawString("WiFi...", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);

  if (setupDebug)
  {
    sprite.setTextSize(4);
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextColor(TFT_SKYBLUE);
    sprite.drawString("DEBUG", LCD_WIDTH / 2, 50);
  }

  Push2TFT();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  WiFi.setAutoReconnect(true);

  if (setupDebug)
  {
    sprite.fillScreen(TFT_BLACK);

    sprite.setTextSize(4);
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextColor(TFT_SKYBLUE);
    sprite.drawString("DEBUG", LCD_WIDTH / 2, 50);

    sprite.setTextSize(3);
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString("WiFi Info", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 45);

    sprite.setTextSize(2);

    sprite.drawString(WiFi.SSID(), LCD_WIDTH / 2, LCD_HEIGHT / 2 - 15);
    sprite.drawString(WiFi.localIP().toString(), LCD_WIDTH / 2, LCD_HEIGHT / 2 + 15);
    sprite.drawString(WiFi.macAddress(), LCD_WIDTH / 2, LCD_HEIGHT / 2 + 45);

    Push2TFT();

    while (ClickButton21.clicks != 1)
      ClickButton21.Update();
    ClickButton21.Update();
  }

  sprite.fillScreen(TFT_BLACK);

  sprite.setTextSize(3);
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("Loading...", LCD_WIDTH / 2, LCD_HEIGHT / 2);
  sprite.drawString("MQTT...", LCD_WIDTH / 2, LCD_HEIGHT / 2 + 30);

  if (setupDebug)
  {
    sprite.setTextSize(4);
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextColor(TFT_SKYBLUE);
    sprite.drawString("DEBUG", LCD_WIDTH / 2, 50);
  }

  Push2TFT();

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setBufferSize(1024);

  mqttConnect();
  client.loop();

  if (setupDebug)
  {
    sprite.fillScreen(TFT_BLACK);

    sprite.setTextSize(4);
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextColor(TFT_SKYBLUE);
    sprite.drawString("DEBUG", LCD_WIDTH / 2, 50);

    sprite.setTextSize(3);
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString("MQTT Info", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 45);

    sprite.setTextSize(2);

    sprite.drawString(MQTT_SERVER, LCD_WIDTH / 2, LCD_HEIGHT / 2 - 15);
    sprite.drawString(String(MQTT_PORT), LCD_WIDTH / 2, LCD_HEIGHT / 2 + 15);
    sprite.drawString(MQTT_USER, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 45);

    Push2TFT();

    while (ClickButton21.clicks != 1)
      ClickButton21.Update();
    ClickButton21.Update();
  }

  // ha.begin(&espClient);

  sprite.fillScreen(TFT_BLACK);
  sprite.setTextSize(3);
  sprite.drawString("Done", LCD_WIDTH / 2, LCD_HEIGHT / 2);

  Push2TFT();

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
  {
    battery.update();
  }

  if (ClickButton21.clicks == -2)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }

  if (ClickButton0.clicks > 1 || ClickButton0.clicks < 0)
  {
    char buf[10];
    sprintf(buf, "%d", ClickButton0.clicks);
    client.publish("esp-remote/btn", buf);
  }

  ///////////////////////////

  switch (currentScreen)
  {

  case 0:
    menuScreen.draw();
    menuScreen.update();
    break;

  case 1:
    rssiMeter.draw();
    rssiMeter.update();
    break;

  case 2:
    deskLamp.draw();
    deskLamp.update();
    break;

  default:
    errorScreen.draw();
    errorScreen.update();
    break;
  }

  Push2TFT();
}
