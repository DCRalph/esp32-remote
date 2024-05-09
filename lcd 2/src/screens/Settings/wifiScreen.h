#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include <WiFi.h>

class WifiScreen : public Screen
{
public:
  WifiScreen(String _name) : Screen(_name){};

  void draw() override;
  void update() override;
};

void WifiScreen::draw()
{
  display.sprite.setTextDatum(TL_DATUM);

  display.sprite.setTextSize(3);
  display.sprite.setTextColor(TFT_ORANGE);
  display.sprite.drawString("Wi-Fi", 10, 30);

  String ssid = WiFi.SSID();
  String ip = WiFi.localIP().toString();
  String mac = WiFi.macAddress();
  String rssi = String(WiFi.RSSI());

  if (WiFi.isConnected())
  {
    display.sprite.setTextSize(2);
    display.sprite.setTextDatum(TL_DATUM);
    display.sprite.setTextColor(TFT_WHITE);

    display.sprite.drawString(rssi, 105, 33);

    display.sprite.setTextColor(TFT_BLUE);
    display.sprite.drawString(ssid, 160, 33);

    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString(ip, 10, 60);
    display.sprite.drawString(mac, 10, 80);
  }
  else
  {
    display.sprite.setTextSize(4);
    display.sprite.setTextDatum(TC_DATUM);
    display.sprite.setTextColor(TFT_RED);
    display.sprite.drawString("No Wi-Fi", LCD_WIDTH / 2, 80);
  }

  char buf[20];
}

void WifiScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.back();
}