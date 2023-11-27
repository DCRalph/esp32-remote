#include <Arduino.h>
#include <TFT_eSPI.h>
#include "driver/rm67162.h"

#define UP_BTN 21
#define DOWN_BTN 0
#define LED_PIN 38
#define BAT_PIN 4

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

float batteryVoltage;
String batteryString;

unsigned long long prevMillis;
int interval = 400;

void setup()
{
  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BAT_PIN, INPUT);

  sprite.createSprite(536, 240);
  sprite.setSwapBytes(1);
  rm67162_init(); // amoled lcd initialization
  lcd_setRotation(1);
}

void loop()
{
  if (millis() - prevMillis > interval)
  {
    prevMillis = millis();
    batteryVoltage = ((analogRead(4) * 2 * 3.3 * 1000) / 4096) / 1000;
    batteryString = "BAT:" + String(batteryVoltage);
  }

  sprite.fillSprite(TFT_BLACK);
  sprite.fillRect(2, 194, 12, 40, 0x6924);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString("AMOLED", 298, -2, 2);

  sprite.drawString(batteryString, 436, 0);
}
