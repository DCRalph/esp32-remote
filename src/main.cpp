#include <Arduino.h>
#include "secrets.h"

#include <TFT_eSPI.h>
#include "driver/rm67162.h"
#include "fonts/NotoSansBold36.h"

#include <WiFi.h>

#include <PubSubClient.h>

#include <ClickButton.h>

#define UP_BTN 21
#define DOWN_BTN 0
#define LED_PIN 38
#define BAT_PIN 4

ClickButton upButton(UP_BTN, LOW);
ClickButton downButton(DOWN_BTN, LOW);

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

WiFiClient espClient;
PubSubClient client(espClient);

float batteryVoltage;
String batteryString;

unsigned long long prevMillis1;
int interval1 = 200;

unsigned long long prevMillis2;
int interval2 = 30000;

uint32_t bg_color = TFT_DARKGREY;

void mqttConnect()
{
  while (!client.connected())
  {
    if (client.connect("ESP32-remote", MQTT_USER, MQTT_PASS))
    {
      client.publish("esp/test", "Hello from ESP32");
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
  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BAT_PIN, INPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  WiFi.setAutoReconnect(true);

  client.setServer(MQTT_SERVER, MQTT_PORT);


  sprite.createSprite(LCD_WIDTH, LCD_HEIGHT);
  sprite.setSwapBytes(1);
  rm67162_init(); // amoled lcd initialization
  lcd_setRotation(TFT_ROT);
  lcd_brightness(255);
}

void loop()
{
  if (!client.connected())
  {
    mqttConnect();
  }
  client.loop();

  upButton.Update();
  downButton.Update();

  if (millis() - prevMillis1 > interval1)
  {
    prevMillis1 = millis();
    batteryVoltage = ((analogRead(4) * 2 * 3.3 * 1000) / 4096) / 1000;
    batteryString = "BAT:" + String(batteryVoltage);
  }

  if (millis() - prevMillis2 > interval2)
  {
    prevMillis2 = millis();

    char batt[10];
    sprintf(batt, "%.2f", batteryVoltage);

    client.publish("esp/batt", batt);
  }

  if (upButton.clicks == 1)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    if (bg_color == TFT_DARKGREY)
      bg_color = TFT_BLUE;
    else
      bg_color = TFT_DARKGREY;
  }

  // if (digitalRead(UP_BTN) == LOW)
  // {
  //   digitalWrite(LED_PIN, HIGH);
  //   bg_color = TFT_BLUE;
  // }
  // else if (digitalRead(DOWN_BTN) == LOW)
  // {
  //   digitalWrite(LED_PIN, LOW);
  //   bg_color = TFT_DARKGREY;
  // }

  sprite.loadFont(NotoSansBold36);
  sprite.fillSprite(TFT_BLACK);

  sprite.fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, TFT_RED);

  sprite.fillSmoothRoundRect(5, 5, LCD_WIDTH - 10, LCD_HEIGHT - 10, 10, bg_color);

  sprite.setTextColor(TFT_WHITE, TFT_BLACK);

  sprite.drawString("AMOLED", 10, 10);
  sprite.drawString(batteryString, 10, 50);
  sprite.drawString("WIFI: " + String(WiFi.localIP().toString()), 10, 90);

  sprite.unloadFont();

  lcd_PushColors(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint16_t *)sprite.getPointer());
}
